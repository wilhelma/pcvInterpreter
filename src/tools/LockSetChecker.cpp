/*
 * LockSetChecker.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "LockSetChecker.h"

#include "fwd/ShadowLock.h"

#include "AccessEvent.h"
#include "AccessInfo.h"
#include "AcquireEvent.h"
#include "AcquireInfo.h"
#include "NewThreadEvent.h"
#include "NewThreadInfo.h"
#include "ReleaseEvent.h"
#include "ReleaseInfo.h"

#include "ShadowThread.h"
#include "ShadowVar.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

LockSetChecker::LockSetChecker(const char *outFile) : outFile_(outFile) {
}

LockSetChecker::~LockSetChecker() {
	dumpRaceEntries(outFile_);
}

void LockSetChecker::NewThread(const NewThreadEvent* event) {
   	ShadowThread* childThread = event->getNewThreadInfo()->childThread;
	// LockSet_u = set of all possible locks
	lockSet_[childThread] =  lockSet_[event->getThread()];
}

void LockSetChecker::Acquire(const AcquireEvent* event) {
	// LockSet_t = LockSet_t + {lock}	
	lockSet_[event->getThread()].insert(event->getAcquireInfo()->lock);
}

void LockSetChecker::Release(const ReleaseEvent* event) {
	// LockSet_t = LockSet_t - {lock}
//	auto lock = ((AcquireEvent*)e)->getAcquireInfo()->lock;
	auto lock = event->getReleaseInfo()->lock;
	lockSet_[event->getThread()].erase(lock); 
}

void LockSetChecker::Access(const AccessEvent* event) {
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
			readVarSet_[ref][threadId].lockset = lockSet_[event->getThread()];

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
			lsIntersect(writeVarSet_[ref].lockset, lockSet_[event->getThread()]);

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
