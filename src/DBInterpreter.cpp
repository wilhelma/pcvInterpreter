/**
 *  @file  DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */
#include "DBInterpreter.h"

#include "fwd/ShadowLock.h"
#include "fwd/ShadowThread.h"
#include "fwd/ShadowVar.h"

#include <iostream>
#include <string>
#include <functional>

#include "SQLStatementIterator.h"

#include "DBTable.h"
#include "DBManager.h"

#include "LockMgr.h"
#include "ThreadMgr.h"

#include "AccessEvent.h"
#include "AccessInfo.h"
#include "AcquireEvent.h"
#include "AcquireInfo.h"
#include "CallEvent.h"
#include "CallInfo.h"
#include "JoinEvent.h"
#include "JoinInfo.h"
#include "NewThreadEvent.h"
#include "NewThreadInfo.h"
#include "ReleaseEvent.h"
#include "ReleaseInfo.h"
#include "ReturnEvent.h"
#include "ReturnInfo.h"
#include "ThreadEndEvent.h"
#include "ThreadEndInfo.h"

#include "EventService.h"
#include "AccessInfo.h"

#include <memory>

// logging system
#include "easylogging++.h"

DBInterpreter::DBInterpreter(std::shared_ptr<EventService> service,
							 std::unique_ptr<LockMgr>&&   lockMgr,
							 std::unique_ptr<ThreadMgr>&& threadMgr) :
	EventService_(service), lockMgr_(std::move(lockMgr)), threadMgr_(std::move(threadMgr))
{
	CallStack_.push(call_t::MAIN);
}


template<typename IdT, typename T>
inline void fill(const std::string& query_string, const DBManager& db, DBTable<IdT, T>& table) {
	std::copy(SQLStatementIterator<T>(db.query(query_string)),
			  SQLStatementIterator<T>::end(), inserter(table));
}

void DBInterpreter::importDB(const std::string& DBPath) { 
	// try to open the database
	DBManager db;
	try {
		db.open(DBPath);
	} catch (const SQLException& e) {
		LOG(FATAL) << e.what();
		std::abort();
	}

    // Read from all the tables in the database
    fill("SELECT * from Access;",        db, AccessTable_);
    fill("SELECT * from Call;",          db, CallTable_);
    fill("SELECT * from File;",          db, FileTable_);
    fill("SELECT * from Function;",      db, FunctionTable_);
    fill("SELECT * from Instruction;",   db, InstructionTable_);
    fill("SELECT * from Loop;",          db, LoopTable_);
    fill("SELECT * from LoopExecution;", db, LoopExecutionTable_);
    fill("SELECT * from LoopIteration;", db, LoopIterationTable_);
    fill("SELECT * from Reference;",     db, ReferenceTable_);
    fill("SELECT * from Segment;",       db, SegmentTable_);
    fill("SELECT * from Thread;",        db, ThreadTable_);

    LOG(TRACE) << "Rows in Access:        " << AccessTable_.size();
    LOG(TRACE) << "Rows in Call:          " << CallTable_.size();
    LOG(TRACE) << "Rows in File:          " << FileTable_.size();
    LOG(TRACE) << "Rows in Function:      " << FunctionTable_.size();
    LOG(TRACE) << "Rows in Instruction:   " << InstructionTable_.size();
    LOG(TRACE) << "Rows in Loop:          " << LoopTable_.size();
    LOG(TRACE) << "Rows in LoopExecution: " << LoopExecutionTable_.size();
    LOG(TRACE) << "Rows in LoopIteration: " << LoopIterationTable_.size();
    LOG(TRACE) << "Rows in Reference:     " << ReferenceTable_.size();
    LOG(TRACE) << "Rows in Segment:       " << SegmentTable_.size();
    LOG(TRACE) << "Rows in Thread:        " << ThreadTable_.size();
}

ErrorCode DBInterpreter::process(const std::string& DBPath) {
	// fill internal tables with the database entries
	try {
		importDB(DBPath);
	} catch (const SQLException& e) {
		// this should only happen if the database can't be closed
		LOG(FATAL) << e.what();
		std::abort();
	}

    // handle start of main routine
    processStart();

    // process database entries
    for (const auto& instruction : InstructionTable_)
        processInstruction(instruction.second);

    // handle end of main routine
    processEnd();

    return ErrorCode::OK;
}


const CAL_ID DBInterpreter::getCallerID(const instruction_t& ins) const {
	auto search = SegmentTable_.find(ins.segment_id);
	if (search == SegmentTable_.cend()) {
		LOG(ERROR) << "Segment " << ins.segment_id
			<< " not found in SegmentTable_";
		return static_cast<CAL_ID>(static_cast<unsigned>(ErrorCode::NO_ENTRY));
	}
	return search->second.call_id;
}

/// @todo If the ID isn't found the program should probably crash. This means
/// the input database was inconsistent. Anyway, here the return value is both
/// the call_id and (in case of error) the ErrorCode. __That's bad design__
const CAL_ID DBInterpreter::getCallID(const instruction_t& ins) const {
    return CallTable_.getInstructionCaller(ins);
}


ErrorCode DBInterpreter::processReturn(const instruction_t& ins,
                                 const call_t& call) {

  while (!CallStack_.isEmpty() &&
         call.id != CallStack_.top()) {
    const CAL_ID topCallId = CallStack_.top();

    auto callIt = CallTable_.find(topCallId);
    if (callIt != CallTable_.end()) {

      // publish a return call event in case of mismatching call id's
      const call_t& topCall = callIt->second;
      ReturnInfo info(topCallId, topCall.end_time);
      auto sThread = getThread(topCall.thread_id);
      ReturnEvent event(sThread, &info);
      getEventService()->publish(&event);

      // publish a thread end event in case of mismatching thread id's
      if (call.thread_id != topCall.thread_id) {
          auto threadIt = ThreadTable_.find(topCall.thread_id);
          if (threadIt != ThreadTable_.end()) {
              const thread_t& thread = threadIt->second;
              ThreadEndInfo  end_info(thread.start_cycle + thread.num_cycles,
                                      sThread->threadId);
              ThreadEndEvent end_event(sThread, &end_info);
              getEventService()->publish(&end_event);
          }
      }
      CallStack_.pop();
    } else {
      return ErrorCode::NO_ENTRY;
    }
  }

	return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAccess(const instruction_t& instruction,
                                 const segment_t& segment,
                                 const call_t& call,
                                 processAccess_t accessFunc) {
  // loop over all memory accesses of the instruction
  const AccessTable::insAccessMap_t& insAccessMap = AccessTable_.getInsAccessMap();
  const auto& avIt = insAccessMap.find(instruction.id);
  if (avIt != insAccessMap.end()) {
    for (const auto& it : avIt->second) {
      auto accessIt = AccessTable_.find(it);
      if (accessIt != AccessTable_.end()) {
        const access_t& access = accessIt->second;
        processAccessGeneric(access.id,
                             access,
                             instruction,
                             segment,
                             call,
                             accessFunc);
      } else {
        LOG(ERROR) << "Access not found: " << it;
        return ErrorCode::NO_ENTRY;
      }
    }
  }
  return ErrorCode::OK;
}

ErrorCode DBInterpreter::processStart() {
  auto ret = ErrorCode::NO_ENTRY;

  auto callIt = CallTable_.find(call_t::MAIN);
  if (callIt != CallTable_.end()) {
    const call_t& call = callIt->second;
    auto threadIt = ThreadTable_.find(call.thread_id);
    if (threadIt != ThreadTable_.end()) {
      const thread_t& thread = threadIt->second;
      processFork(thread);
      processCall(call, static_cast<LIN_NO>(0), static_cast<SEG_ID>(0));
      ret = ErrorCode::OK;
    }
  }

  return ret;
}

ErrorCode DBInterpreter::processEnd() {
  auto ret = ErrorCode::NO_ENTRY;

  auto callIt = CallTable_.find(call_t::MAIN);
  if (callIt != CallTable_.end()) {
    const call_t& call = callIt->second;
    auto threadIt = ThreadTable_.find(call.thread_id);
    if (threadIt != ThreadTable_.end()) {
      const thread_t& thread = threadIt->second;

      // publish a return call event for main
      auto sThread = getThread(call.thread_id);
      ReturnInfo info(call.id, call.end_time);
      ReturnEvent event(sThread, &info);
      getEventService()->publish(&event);

      // publish a thread end event for the main thread
      ThreadEndInfo  end_info(static_cast<TIME>(
                                  thread.start_cycle + thread.num_cycles),
                              sThread->threadId);
      ThreadEndEvent end_event(sThread, &end_info);
      getEventService()->publish(&end_event);
      ret = ErrorCode::OK;
    }
  }

  return ret;
}

ErrorCode DBInterpreter::processInstruction(const instruction_t& ins) {
  auto ret = ErrorCode::NO_ENTRY;

  auto segmentIt = SegmentTable_.find(ins.segment_id);
  if (segmentIt != SegmentTable_.end()) {
    const segment_t& segment = segmentIt->second;
    
    auto callIt = CallTable_.find(segment.call_id);
    if (callIt != CallTable_.end()) {
      const call_t& call = callIt->second;

      // handle call/thread stack returns
      processReturn(ins, call);

      switch (ins.instruction_type) {
      case InstructionType::CALL:
        ret = processCall(ins);
        break;
      case InstructionType::ACCESS:
        ret = processAccess(ins, segment, call, &DBInterpreter::processMemAccess);
        break;
      case InstructionType::ACQUIRE:
        ret = processAcquire(ins);
        break;
      case InstructionType::RELEASE:
        ret = processRelease(ins);
        break;
      case InstructionType::FORK:
        for (const auto& it : ThreadTable_) {
          const thread_t& thread = it.second;
          if (ins.id == thread.create_instruction_id) {
            ret = processFork(thread);
          }
        }
        break;
      case InstructionType::JOIN:
        for (const auto& it : ThreadTable_) {
          const thread_t& thread = it.second;
          if (ins.id == thread.join_instruction_id) {
              ret = processJoin(ins, segment, call, thread);
          }
        }
        break;
      default:
        ret = ErrorCode::NO_ENTRY;
      }
    }
  }

  return ret;
}

size_t getHash(unsigned funId, unsigned lineNo) {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

//int DBInterpreter::processSegment(SEG_ID segmentId,
//                                  const segment_t& seg,
//                                  const instruction_t& ins) {
//
//    auto search = CallTable_.find(seg.call_id);
//    if (search != CallTable_.end()) {
//        if (!processCall(seg.call_id, search->second, seg, ins))
//            return 1;
//    } else {
//        LOG(ERROR) << "Call not found: " << seg.call_id;
//        return 1;
//    }
//
//    return 0;
//}

ErrorCode DBInterpreter::processCall(const call_t& call, LIN_NO line, SEG_ID segId) {
  auto ret = ErrorCode::NO_ENTRY;

  auto functionIt = FunctionTable_.find(call.function_id);
  if (functionIt != FunctionTable_.end()) {
    const function_t& function = functionIt->second;

    switch(function.type) {
    case FunctionType::FUNCTION:
    case FunctionType::METHOD:
    {
      // serch for the file where the call has happened from
      auto fileIt = FileTable_.find(function.file_id);
      if (fileIt != FileTable_.end()) {
        const file_t& file = fileIt->second;

        CallInfo info( static_cast<CALLSITE>(getHash(call.function_id, line)),
                       call.start_time,
                       static_cast<TIME>(call.end_time - call.start_time),
                       function.name,
                       segId,
                       function.type,
                       file.file_name,
                       file.file_path);

        auto thread = getThread(call.thread_id);
        CallEvent event(thread, &info);
        getEventService()->publish(&event);
        CallStack_.push(call.id);
        ret = ErrorCode::OK;
      }
      break;
    }
    default:
      break;
    }
  }

  return ret;
}

ErrorCode DBInterpreter::processCall(const instruction_t& ins) {
  auto ret = ErrorCode::NO_ENTRY;

  auto callIt = CallTable_.find(getCallID(ins));
  if (callIt != CallTable_.end()) {
    const call_t& call = callIt->second;
    ret = processCall(call, ins.line_number, ins.segment_id);
  }

  return ret;
}

ErrorCode DBInterpreter::processAccessGeneric(ACC_ID accessId,
                                        const access_t& access,
                                        const instruction_t& instruction,
                                        const segment_t& segment,
                                        const call_t& call,
                                        processAccess_t func) {

    REF_ID refId = access.reference_id;
    auto search = ReferenceTable_.find(refId);
    if ( search == ReferenceTable_.end() ) {
        LOG(ERROR) << "Reference not found: " << access.reference_id;
        return ErrorCode::NO_ENTRY;
	}

    return (this->* func)(accessId, access, instruction, segment,
                       call, search->second);
}

ErrorCode DBInterpreter::processMemAccess(ACC_ID accessId,
                                    const access_t& access,
                                    const instruction_t& instruction,
                                    const segment_t& segment,
                                    const call_t& call,
                                    const reference_t& reference) {

    ShadowVar *var = 0;
    auto searchVar = _shadowVarMap.find(reference.id);
    if ( searchVar != _shadowVarMap.end() ) {
        var = searchVar->second;
    } else {
        var = new ShadowVar(reference.memory_type,
                            reference.id,
                            reference.size,
                            reference.name);
        _shadowVarMap[reference.id] = var;
    }

    auto thread = getThread(call.thread_id);
    AccessInfo info( access.access_type,
                     var,
                     instruction.id);
    AccessEvent event( thread, &info );
    getEventService()->publish( &event );

    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processAcquire(const instruction_t& ins) {
    auto callIt = CallTable_.find(getCallID(ins));
    if (callIt != CallTable_.end()) {
      const call_t& call = callIt->second;
      for (auto accIt : AccessTable_) {
        const access_t& access = accIt.second;
        if (access.instruction_id == ins.id) {
          auto refIt = ReferenceTable_.find(access.reference_id);
          auto lock = getLock(refIt->second.id);
          if (lock != nullptr) {
            auto thread = getThread(call.thread_id);
            AcquireInfo info(lock, call.start_time);
            AcquireEvent event( thread, &info );
            getEventService()->publish( &event );
            return ErrorCode::OK;
          }
        }
      }
    }
    return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processRelease(const instruction_t& ins) {
  auto callIt = CallTable_.find(getCallID(ins));
  if (callIt != CallTable_.end()) {
    const call_t& call = callIt->second;
    for (auto accIt : AccessTable_) {
      const access_t& access = accIt.second;
      if (access.instruction_id == ins.id) {
        auto refIt = ReferenceTable_.find(access.reference_id);
        auto lock = getLock(refIt->second.id);
        if (lock != nullptr) {
          auto thread = getThread(call.thread_id);
          ReleaseInfo info(lock, call.start_time);
          ReleaseEvent event( thread, &info );
          getEventService()->publish( &event );
          return ErrorCode::OK;
        }
      }
    }
  }
  return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processFork(const thread_t& thread) {
    auto pT = getThread(thread.parent_thread_id);
    auto cT = getThread(thread.id);
    NewThreadInfo info(cT, pT, thread.start_cycle, thread.num_cycles);
    NewThreadEvent event( pT, &info );
    getEventService()->publish( &event );
    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processJoin(const instruction_t& instruction,
                               const segment_t& segment,
                               const call_t& call,
                               const thread_t& thread) {

    auto pT = getThread(thread.parent_thread_id);
    auto cT = getThread(thread.id);
    JoinInfo info(cT, pT, call.end_time);
    JoinEvent event( pT, &info );
    getEventService()->publish( &event );

    return ErrorCode::OK;
}
