	/*
 * LockSetChecker.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "LockSetChecker.h"
#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

LockSetChecker::LockSetChecker(const char *outFile) : outFile_(outFile) {
}

LockSetChecker::~LockSetChecker() {

	dumpRaceEntries(outFile_);
}

void LockSetChecker::create(const Event* e) {

   	ShadowThread* childThread = 
		dynamic_cast<const NewThreadEvent*>(e)->getNewThreadInfo()->childThread;

	// LockSet_u = set of all possible locks
	lockSet_[childThread] =  lockSet_[e->getThread()];
}

void LockSetChecker::join(const Event* e) {

}

void LockSetChecker::acquire(const Event* e) {

	// LockSet_t = LockSet_t + {lock}	
	lockSet_[e->getThread()].insert(((AcquireEvent*)e)->getAcquireInfo()->lock);
}

void LockSetChecker::release(const Event* e) {

	// LockSet_t = LockSet_t - {lock}
	auto lock = ((AcquireEvent*)e)->getAcquireInfo()->lock;
	lockSet_[e->getThread()].erase(lock); 
}

void LockSetChecker::access(const Event* e) {

	const AccessEvent *event = dynamic_cast<const AccessEvent*>(e);
	const REF_ID ref = event->getAccessInfo()->var->id;
	const TRD_ID threadId = event->getThread()->threadId;

	if (event->getAccessInfo()->var->type == ReferenceType::STACK)
		return;

	switch(event->getAccessInfo()->type) {
	case AccessType::READ:
		{
			readVarSet_[ref][threadId].instruction = 
				event->getAccessInfo()->instructionID;

			// R_x[t].lockset = LockSet_t
			readVarSet_[ref][threadId].lockset = lockSet_[e->getThread()];

			if (lsIsEmptySet( readVarSet_[ref][threadId].lockset,
								  writeVarSet_[ref].lockset) ) {

					raceEntries_.push_back(
							std::unique_ptr<RaceEntry_>(new RaceEntry_(
									WRITE_READ,
									writeVarSet_[ref].instruction,
									event->getAccessInfo()->instructionID,
									ref)
							));
					std::cout << "race detected..1" << std::endl;
			}
		}
		break;

	case AccessType::WRITE:
		{	
			// W_x.lockset = W_x.lockset intersect Lockset_t
			lsIntersect(writeVarSet_[ref].lockset, lockSet_[e->getThread()]);

			// check W_x.lockset = empty
			if (writeVarSet_[ref].lockset.empty())	{

				raceEntries_.push_back(
						std::unique_ptr<RaceEntry_>(new RaceEntry_(
								WRITE_WRITE,
								writeVarSet_[ref].instruction,
								event->getAccessInfo()->instructionID,
								ref)
						));
				std::cout << "race detected..2" << std::endl;
			}
		}
															 
	default:
		break;
	}

}

void LockSetChecker::dumpRaceEntries(const char* fileName) const {

	rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value array(rapidjson::kArrayType);

    for (const auto& race : raceEntries_) {

    	rapidjson::Value object(rapidjson::kObjectType);
    	object.AddMember("type", race->type, allocator);
    	object.AddMember("1st", race->firstInstruction, allocator);
    	object.AddMember("2nd", race->secondInstruction, allocator);
    	object.AddMember("id", race->id, allocator);

    	array.PushBack(object, allocator);
    }

    doc.AddMember("races", array, allocator);

    // 3. Stringify the DOM
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    // 4. Write to the file.
    std::ofstream file;
    file.open (fileName);
    file << buffer.GetString();
    file.close();
}

void LockSetChecker::call(const Event* e) { }

bool LockSetChecker::lsIsEmptySet(const LockSet_& lhs,
									 const LockSet_& rhs) const {

	LockSet_ result;
	std::set_intersection(lhs.begin(), lhs.end(),
						  rhs.begin(), rhs.end(),
							std::inserter(result, result.begin()));

	return result.empty();
}

void LockSetChecker::lsIntersect(LockSet_& lhs, const LockSet_& rhs) const {

	LockSet_ result;
	std::set_intersection(lhs.begin(), lhs.end(),
						  rhs.begin(), rhs.end(),
							std::inserter(result, result.begin()));

	lhs = result;
}
