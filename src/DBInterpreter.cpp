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

#include "SQLException.h"

#include <iostream>
#include <string>
#include <functional>
#include <cassert>

#include "LockMgr.h"
#include "ThreadMgr.h"

#include "Access.h"
#include "Call.h"
#include "File.h"
#include "Function.h"
#include "Instruction.h"
#include "Loop.h"
#include "LoopExecution.h"
#include "LoopIteration.h"
#include "Reference.h"
#include "Segment.h"
#include "Thread.h"

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

#include "Database.h"

#include "EventService.h"
#include "Types.h"

#include <memory>

// logging system
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// Helper function to initialize the log.
void initialize_logger(std::string&& logFileName) {
	namespace logging = boost::log;
	namespace expr    = boost::log::expressions;

	logging::add_file_log(
			logging::keywords::file_name = logFileName,
			logging::keywords::format = (
				expr::stream
				<< expr::attr< unsigned int >("LineID")
				<< ": <" << logging::trivial::severity
				<< "> " << expr::smessage
				)	
			);

	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::trace);
	logging::add_common_attributes();
}

size_t getHash(unsigned funId, unsigned lineNo) {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

DBInterpreter::DBInterpreter(std::string&& logFile,
                             std::shared_ptr<EventService> service,
							 std::unique_ptr<LockMgr>&&   lockMgr,
							 std::unique_ptr<ThreadMgr>&& threadMgr) :
  lastEventTime_(0), EventService_(service), lockMgr_(std::move(lockMgr)),
  threadMgr_(std::move(threadMgr))
{
	initialize_logger(std::move(logFile));
}

DBInterpreter::~DBInterpreter() = default;

ErrorCode DBInterpreter::process(const std::string& DBPath) {
	// fill internal tables with the database entries
	try {
		Database_ = load_database(DBPath);
	} catch (const SQLException& e) {
		// this should only happen if the database can't be closed
		BOOST_LOG_TRIVIAL(fatal) << e.what();
		std::abort();
	}

    // handle start of main routine
    processStart();

    // process database entries
    for (const auto& instruction : database()->instructionTable())
        processInstruction(instruction.second);

    // handle end of main routine
    processEnd();

    return ErrorCode::OK;
}

const CAL_ID DBInterpreter::getCallerID(const instruction_t& ins) const {
	auto search = database()->segmentTable().find(ins.segment_id);
	if (search == database()->segmentTable().cend()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id
			<< " not found in SegmentTable";
		return static_cast<CAL_ID>(static_cast<unsigned>(ErrorCode::NO_ENTRY));
	}
	return search->second.call_id;
}

/// @todo If the ID isn't found the program should probably crash. This means
/// the input database was inconsistent. Anyway, here the return value is both
/// the call_id and (in case of error) the ErrorCode. __That's bad design__
const CAL_ID DBInterpreter::getCallID(const instruction_t& ins) const {
    return database()->callTable().getInstructionCaller(ins);
//    // look for the call id of the instruction
//    CAL_ID call_id(0);
//    bool found = false;
//    for (const auto& it : database()->callTable()) {
//        // if there's a call whose instruction id is the same as
//        // ins.id, get its id
//        if (it.second.instruction_id == ins.id) {
//            call_id = it.second.id;
//            found = true;
//            break;
//        }
//    }
//
//    // if the iterator reached the end, no entry has been found in callT_
//    if (!found) {
//        BOOST_LOG_TRIVIAL(error) << "Call table has no element whose instruction id is: "
//            << ins.id;
//        return static_cast<CAL_ID>(static_cast<unsigned>(ErrorCode::NO_ENTRY));
//    }
//
//    //	std::cerr << "[getCallID]     call_id = " << call_id << std::endl;
//    return call_id;
}


ErrorCode DBInterpreter::processReturn(const instruction_t& ins,
                                       const call_t& call) {
  TRD_ID returnThread = NO_TRD_ID;

  CAL_ID topCallId =CallStack_.top();
  while (!CallStack_.isEmpty() && call.id != topCallId) {
    auto callIt = database()->callTable().find(topCallId);
    if (callIt != database()->callTable().end()) {
      const call_t& topCall = callIt->second;

      if (returnThread != NO_TRD_ID && returnThread != topCall.thread_id) {
        publishThreadReturn(topCall.thread_id);
        returnThread = NO_TRD_ID;
      }

      publishCallReturn(topCall);

      if (call.thread_id != topCall.thread_id)
        returnThread = topCall.thread_id;

      CallStack_.pop();
      topCallId = CallStack_.top();
    } else {
      return ErrorCode::NO_ENTRY;
    }
  }

  if (returnThread != NO_TRD_ID)
    publishThreadReturn(returnThread);

	return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishCallReturn(const call_t& topCall) {
  assert(lastEventTime_ <= topCall.end_time);

  auto sThread = getThread(topCall.thread_id);
  ReturnInfo info(topCall.id, topCall.function_id, topCall.end_time);
  ReturnEvent event(sThread, &info);
  getEventService()->publish(&event);
  lastEventTime_ = topCall.end_time;

  return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishThreadReturn(TRD_ID threadId) {
  auto threadIt = database()->threadTable().find(threadId);
  if (threadIt != database()->threadTable().end()) {
    const thread_t& thread = threadIt->second;
    const TIME threadEndTime = thread.start_cycle + thread.num_cycles;
    assert(lastEventTime_ <= threadEndTime);
    ThreadEndInfo  end_info(threadEndTime, threadId);
    auto sThread = getThread(threadId);
    ThreadEndEvent end_event(sThread, &end_info);
    getEventService()->publish(&end_event);
    lastEventTime_ = threadEndTime;
    lastThreadId = thread.parent_thread_id;
  } else {
    return ErrorCode::NO_ENTRY;
  }

  return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAccess(const instruction_t& instruction,
                                 const segment_t& segment,
                                 const call_t& call,
                                 processAccess_t accessFunc) {
  // loop over all memory accesses of the instruction
  const AccessTable::insAccessMap_t& insAccessMap = database()->accessTable().getInsAccessMap();
  const auto& avIt = insAccessMap.find(instruction.id);
  if (avIt != insAccessMap.end()) {
    for (const auto& it : avIt->second) {
      auto accessIt = database()->accessTable().find(it);
      if (accessIt != database()->accessTable().end()) {
        const access_t& access = accessIt->second;
        processAccessGeneric(access.id,
                             access,
                             instruction,
                             segment,
                             call,
                             accessFunc);
      } else {
        BOOST_LOG_TRIVIAL(error) << "Access not found: " << it;
        return ErrorCode::NO_ENTRY;
      }
    }
  }
  return ErrorCode::OK;
}

ErrorCode DBInterpreter::processStart() {
  auto ret = ErrorCode::NO_ENTRY;

  auto callIt = database()->callTable().find(call_t::MAIN);
  if (callIt != database()->callTable().end()) {
    const call_t& call = callIt->second;
    auto threadIt = database()->threadTable().find(call.thread_id);
    if (threadIt != database()->threadTable().end()) {
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

  // close final calls
  while (!CallStack_.isEmpty()) {
    const CAL_ID topCallId = CallStack_.pop();
    auto callIt = database()->callTable().find(topCallId);
    if (callIt != database()->callTable().end()) {
      const call_t& call = callIt->second;
      auto sThread = getThread(call.thread_id);

      if (call.thread_id != lastThreadId) {
        // publish a thread end event
        auto threadIt = database()->threadTable().find(lastThreadId);
        if (threadIt != database()->threadTable().end()) {
          const thread_t& thread = threadIt->second;
          ThreadEndInfo end_info(static_cast<TIME>(
                                 thread.start_cycle + thread.num_cycles),
                                 thread.id);
          ThreadEndEvent end_event(sThread, &end_info);
          getEventService()->publish(&end_event);
          lastThreadId = call.thread_id;
        }
      }

      ReturnInfo info (call.id, call.function_id, call.end_time);
      ReturnEvent event(sThread, &info);
      getEventService()->publish(&event);
      ret = ErrorCode::OK;
    }
  }

  return ret;
}

ErrorCode DBInterpreter::processInstruction(const instruction_t& ins) {
  auto ret = ErrorCode::NO_ENTRY;

  auto segmentIt = database()->segmentTable().find(ins.segment_id);
  if (segmentIt != database()->segmentTable().end()) {
    const segment_t& segment = segmentIt->second;
    
    auto callIt = database()->callTable().find(segment.call_id);
    if (callIt != database()->callTable().end()) {
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
        for (const auto& it : database()->threadTable()) {
          const thread_t& thread = it.second;
          if (ins.id == thread.create_instruction_id) {
            ret = processFork(thread);
          }
        }
        break;
      case InstructionType::JOIN:
        for (const auto& it : database()->threadTable()) {
          const thread_t& thread = it.second;
          if (ins.id == thread.join_instruction_id) {
              ret = processJoin(ins, thread);
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

//int DBInterpreter::processSegment(SEG_ID segmentId,
//                                  const segment_t& seg,
//                                  const instruction_t& ins) {
//
//    auto search = database()->callTable().find(seg.call_id);
//    if (search != database()->callTable().end()) {
//        if (!processCall(seg.call_id, search->second, seg, ins))
//            return 1;
//    } else {
//        BOOST_LOG_TRIVIAL(error) << "Call not found: " << seg.call_id;
//        return 1;
//    }
//
//    return 0;
//}

ErrorCode DBInterpreter::processCall(const call_t& call, LIN_NO line, SEG_ID segId) {
  assert(lastEventTime_ <= call.start_time);

  auto ret = ErrorCode::NO_ENTRY;

  auto functionIt = database()->functionTable().find(call.function_id);
  if (functionIt != database()->functionTable().end()) {
    const function_t& function = functionIt->second;

    switch(function.type) {
    case FunctionType::FUNCTION:
    case FunctionType::METHOD:
    {
      // serch for the file where the call has happened from
      auto fileIt = database()->fileTable().find(function.file_id);
      if (fileIt != database()->fileTable().end()) {
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
        lastEventTime_ = call.start_time;
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

  auto callIt = database()->callTable().find(getCallID(ins));
  if (callIt != database()->callTable().end()) {
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
    auto search = database()->referenceTable().find(refId);
    if ( search == database()->referenceTable().end() ) {
        BOOST_LOG_TRIVIAL(error) << "Reference not found: " << access.reference_id;
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
    auto callIt = database()->callTable().find(getCallID(ins));
    if (callIt != database()->callTable().end()) {
      const call_t& call = callIt->second;
      assert(lastEventTime_ <= call.start_time);
      for (auto accIt : database()->accessTable()) {
        const access_t& access = accIt.second;
        if (access.instruction_id == ins.id) {
          auto refIt = database()->referenceTable().find(access.reference_id);
          auto lock = getLock(refIt->second.id);
          if (lock != nullptr) {
            auto thread = getThread(call.thread_id);
            AcquireInfo info(lock, call.start_time);
            AcquireEvent event( thread, &info );
            getEventService()->publish( &event );
            lastEventTime_ = call.start_time;
            return ErrorCode::OK;
          }
        }
      }
    }
    return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processRelease(const instruction_t& ins) {
  auto callIt = database()->callTable().find(getCallID(ins));
  if (callIt != database()->callTable().end()) {
    const call_t& call = callIt->second;
    assert(lastEventTime_ <= call.start_time);
    for (auto accIt : database()->accessTable()) {
      const access_t& access = accIt.second;
      if (access.instruction_id == ins.id) {
        auto refIt = database()->referenceTable().find(access.reference_id);
        auto lock = getLock(refIt->second.id);
        if (lock != nullptr) {
          auto thread = getThread(call.thread_id);
          ReleaseInfo info(lock, call.start_time);
          ReleaseEvent event( thread, &info );
          getEventService()->publish( &event );
          lastEventTime_ = call.start_time;
          return ErrorCode::OK;
        }
      }
    }
  }
  return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processFork(const thread_t& thread) {
    assert (lastEventTime_ <= thread.start_cycle);
    auto pT = getThread(thread.parent_thread_id);
    auto cT = getThread(thread.id);
    NewThreadInfo info(cT, pT, thread.start_cycle, thread.num_cycles);
    NewThreadEvent event( pT, &info );
    getEventService()->publish( &event );
    lastEventTime_ = thread.start_cycle;
    lastThreadId = thread.id;
    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processJoin(const instruction_t& ins,
                                     const thread_t& thread) {

    auto pT = getThread(thread.parent_thread_id);
    auto cT = getThread(thread.id);
    JoinInfo info(cT, pT, lastEventTime_);
    JoinEvent event( pT, &info );
    getEventService()->publish( &event );
    lastThreadId = thread.parent_thread_id;

    return ErrorCode::OK;
}

std::unique_ptr<DBInterpreter> make_DBInterpreter(std::string&& logFileName) {
	return std::make_unique<DBInterpreter>(
			std::move(logFileName), std::make_shared<EventService>(),
			std::make_unique<LockMgr>(), std::make_unique<ThreadMgr>());
}
