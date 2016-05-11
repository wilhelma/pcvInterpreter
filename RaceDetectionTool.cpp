/*
 * RaceDetectionTool.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "RaceDetectionTool.h"
#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

RaceDetectionTool::RaceDetectionTool(const char *outFile) : outFile_(outFile) {
		threadVC_[(TRD_ID)0].fill(0);
		threadVC_[(TRD_ID)0][0] = 1;
}

RaceDetectionTool::~RaceDetectionTool() {

	dumpRaceEntries(outFile_);
}

void RaceDetectionTool::create(const Event* e) {

   	ShadowThread* childThread = 
		dynamic_cast<const NewThreadEvent*>(e)->getNewThreadInfo()->childThread;

	if (threadVC_.find(childThread->threadId) == threadVC_.end()) {
		threadVC_[childThread->threadId].fill(0);
		threadVC_[childThread->threadId][childThread->threadId] = 1;	
	}

	// LockSet_u = set of all possible locks
	lockSet_[childThread] =  lockSet_[e->getThread()];

	// VC_u = Vc_u # VC_t
	vcMerge( threadVC_[childThread->threadId],
			 threadVC_[e->getThread()->threadId] );

	// VC_t[t] = VC_t[t] + 1
	threadVC_[e->getThread()->threadId][e->getThread()->threadId]++;
}

void RaceDetectionTool::join(const Event* e) {

	// VC_t = VC_t # VC_u
	vcMerge( threadVC_[e->getThread()->threadId],
		threadVC_[((JoinEvent*)e)->getJoinInfo()->childThread->threadId] );

	// VC_u[u] = VC_u[u] + 1
	TRD_ID id = ((JoinEvent*)e)->getJoinInfo()->childThread->threadId;
	threadVC_[id][id]++;
}

void RaceDetectionTool::acquire(const Event* e) {

	// LockSet_t = LockSet_t + {lock}	
	lockSet_[e->getThread()].insert(((AcquireEvent*)e)->getAcquireInfo()->lock);
}

void RaceDetectionTool::release(const Event* e) {

	// LockSet_t = LockSet_t - {lock}
	auto lock = ((AcquireEvent*)e)->getAcquireInfo()->lock;
	lockSet_[e->getThread()].erase(lock); 

	// VC_t[t] = VC_t[t] + 1
	threadVC_[e->getThread()->threadId][e->getThread()->threadId]++;
}

void RaceDetectionTool::access(const Event* e) {

	const AccessEvent *event = dynamic_cast<const AccessEvent*>(e);
	const REF_ID ref = event->getAccessInfo()->var->id;
	Epoch_ epoch(e->getThread()->threadId,
				 threadVC_[e->getThread()->threadId][e->getThread()->threadId]);
	const TRD_ID threadId = event->getThread()->threadId;

	if (event->getAccessInfo()->var->type == ReferenceType::STACK)
		return;

	switch(event->getAccessInfo()->type) {
	case AccessType::READ:
		{
			
			// if epoch(t) != R_x[t].epoch
			if (epoch == readVarSet_[ref][threadId].epoch)
				return;

			//auto searchRead = readVarSet_.find(ref);
			//if (searchRead != readVarSet_.end()) {
			//	auto searchThread = searchRead->second.find(e->getThread()->threadId);
			//	if (searchThread != searchRead->second.end())
			//		if (epoch == searchThread->second.epoch)
			//			return;
			//}

			// if epoch(t) != W_x.epoch
			if (epoch == writeVarSet_[ref].epoch)
				return;

			//auto searchWrite = writeVarSet_.find(ref);
			//if (searchWrite != writeVarSet_.end())
			//	if (epoch == searchWrite->second.epoch)
			//		return;

			// R_x[t].epoch = epoch(t)
			readVarSet_[ref][threadId].epoch = epoch;
			readVarSet_[ref][threadId].instruction = 
				event->getAccessInfo()->instructionID;

			// R_x[t].lockset = LockSet_t
			readVarSet_[ref][threadId].lockset = lockSet_[e->getThread()];

			// check if W_x.epoch > VC_t
			if ( !vcLEQ(writeVarSet_[ref].epoch, threadVC_[threadId]) ) {
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
		}
		break;

	case AccessType::WRITE:
		
		// if epoch(t) != W_x.epoch
		if (epoch == writeVarSet_[ref].epoch)
			return;

		// if W_x.epoch > Lockset_t
		if ( !vcLEQ(writeVarSet_[ref].epoch, threadVC_[threadId]) ) {

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
															 
		} else {

			// W_x.lockset = Lockset_t
			writeVarSet_[ref].lockset = lockSet_[e->getThread()];
		}

		// W_x.epoch = epoch(t)
		writeVarSet_[ref].epoch = epoch;
		writeVarSet_[ref].instruction = event->getAccessInfo()->instructionID;

		// forall threads t' in read map R_x do
		for (auto tp : readVarSet_[ref]) {

			// if R_x[t'].epoch > VC_t then
			if ( !vcLEQ(tp.second.epoch, threadVC_[threadId]) ) {
				
				// check R_x[t'].lockset intersect Lockset_t = empty
				if ( lsIsEmptySet(readVarSet_[ref][tp.first].lockset, lockSet_[e->getThread()]) ) {

					raceEntries_.push_back(
							std::unique_ptr<RaceEntry_>(new RaceEntry_(
									READ_WRITE,
									tp.second.instruction,
									event->getAccessInfo()->instructionID,
									ref)
							));
					std::cout << "race detected..3" << std::endl;
				}
			}
		}

		// R_x = empty
		readVarSet_[ref].clear();
		break;

	default:
		break;
	}


	//LockSet_* ls = (LockSet_*)((const AccessEvent*)e)->getAccessInfo()->var->ptr;

	//if (ls == nullptr) {
	//	ls = new LockSet_();
	//	((const AccessEvent*)e)->getAccessInfo()->var->ptr = (void*)ls;
	//}

	//LockSet_ result;
	//auto search = lockSet_.find(e->getThread());  
	//if (search != lockSet_.end()) {
	//	std::set_intersection(ls->begin(), ls->end(),
	//						  search->second.begin(), search->second.end(),
	//						  std::inserter(result, result.begin()));

	//}

	//*ls = result;
}

void RaceDetectionTool::dumpRaceEntries(const char* fileName) const {

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

void RaceDetectionTool::call(const Event* e) { }

void RaceDetectionTool::vcMerge(VectorClock_& lhs,
								const VectorClock_& rhs) const {

	for (unsigned i = 0; i < lhs.size(); ++i) {
		if (lhs[i] < rhs[i])
			lhs[i] = rhs[i];
	}
}

bool RaceDetectionTool::vcLEQ(const Epoch_& epoch,
							  const VectorClock_& vc) const {

	return (epoch.clock <= vc[epoch.threadId]);
}

bool RaceDetectionTool::lsIsEmptySet(const LockSet_& lhs,
									 const LockSet_& rhs) const {

	LockSet_ result;
	std::set_intersection(lhs.begin(), lhs.end(),
						  rhs.begin(), rhs.end(),
							std::inserter(result, result.begin()));

	return result.empty();
}

void RaceDetectionTool::lsIntersect(LockSet_& lhs, const LockSet_& rhs) const {

	LockSet_ result;
	std::set_intersection(lhs.begin(), lhs.end(),
						  rhs.begin(), rhs.end(),
							std::inserter(result, result.begin()));

	lhs = result;
}
