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

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include "DBTable.h"

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

#include "DBTable.h"
#include "Types.h"

int DBInterpreter::loadDB(const char* path, sqlite3 **db) const {
    if (sqlite3_open_v2(path, db,
            SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL) != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(fatal) << "Can't open " << path << " - error: "
                                 << sqlite3_errmsg(*db);
        sqlite3_close(*db);
        return IN_ABORT;
    } else {
        BOOST_LOG_TRIVIAL(trace) << "successfully opened " << path;
        return IN_OK;
    }
}

int DBInterpreter::importDB(sqlite3 **db) {

    int rc;

    // Read from all the tables in the database
    if ((rc = fillGeneric("SELECT * from Access;",
                          db, &accessTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Call;",
                          db, &callTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from File;",
                          db, &fileTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Function;",
                          db, &functionTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Instruction;",
                          db, &instructionTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Loop;",
                          db, &loopTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from LoopExecution;",
                          db, &loopExecutionTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from LoopIteration;",
                          db, &loopIterationTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Reference;",
                          db, &referenceTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Segment;",
                          db, &segmentTable)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Thread;",
                          db, &threadTable)) != 0) return rc;

    BOOST_LOG_TRIVIAL(trace) << "Rows in Access:        " << accessTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Call:          " << callTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in File:          " << fileTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Function:      " << functionTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Instruction:   " << instructionTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Loop:          " << loopTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in LoopExecution: " << loopExecutionTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in LoopIteration: " << loopIterationTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Reference:     " << referenceTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Segment:       " << segmentTable.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Thread:        " << threadTable.size();
    return 0;
}

int DBInterpreter::closeDB(sqlite3 **db) const {

    if ( sqlite3_close(*db) != SQLITE_OK)
        BOOST_LOG_TRIVIAL(fatal) <<  "Can't close database - error: "
                                 <<  sqlite3_errmsg(*db);

    return IN_OK;
}

/// The main process routine. Loops over instructions.
int DBInterpreter::process() {

    sqlite3 *db;

    // open the database
    if ( loadDB(_dbPath, &db) != IN_OK ) {
        return IN_ABORT;
    }

    // fill the internal maps with database entries
    int rc = importDB(&db);
    if ( rc != IN_OK) {
        BOOST_LOG_TRIVIAL(error) << "Can't fill internal structures."
                                 << " Error code: " << rc;
        return IN_ABORT;
    }

    // handle start of main routine
    processStart();

    // process database entries
    for (const auto& instruction : instructionTable)
        processInstruction(instruction.second);

    // handle end of main routine
    processEnd();

    closeDB(&db);
    return 0;
}

const CAL_ID DBInterpreter::getCallerID(const instruction_t& ins) const {
	auto search = segmentTable.find(ins.segment_id);
	if (search == segmentTable.cend()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id
			<< " not found in segmentTable";
		return static_cast<CAL_ID>(IN_NO_ENTRY);
	}
	return search->second.call_id;
}

const CAL_ID DBInterpreter::getCallID(const instruction_t& ins) const {
	// look for the call id of the instruction
	CAL_ID call_id(0);
	bool found = false;
	for (const auto& it : callTable) {
		// if there's a call whose instruction id is the same as
		// ins.instruction_id, get its id
		if (it.second.instruction_id == ins.instruction_id) {
			call_id = it.second.sql_id;
			found = true;
			break;
		}
	}

	// if the iterator reached the end, no entry has been found in callT_
	if (!found) {
		BOOST_LOG_TRIVIAL(error) << "Call table has no element whose instruction id is: "
			<< ins.instruction_id;
		return static_cast<CAL_ID>(IN_NO_ENTRY);
	}

	return call_id;
}


int DBInterpreter::processReturn(const instruction_t& ins,
                                 const call_t& call) {

  while (!callStack_.isEmpty() &&
         call.sql_id != callStack_.top()) {
    const CAL_ID topCallId = callStack_.top();

    auto callIt = callTable.find(topCallId);
    if (callIt != callTable.end()) {

      // publish a return call event in case of mismatching call id's
      const call_t& topCall = callIt->second;
      ReturnInfo info(topCallId, topCall.end_time);
      ShadowThread* thread = threadMgr_->getThread(topCall.thread_id);
      ReturnEvent event(thread, &info);
      _eventService->publish(&event);

      // publish a thread end event in case of mismatching thread id's
      if (call.thread_id != topCall.thread_id) {
        ThreadEndInfo  end_info(topCall.end_time, thread->threadId);
        ThreadEndEvent end_event(thread, &end_info);
        _eventService->publish(&end_event);
      }
      callStack_.pop();
    } else {
      return IN_NO_ENTRY;
    }
  }

	return IN_OK;
}


int DBInterpreter::processAccess(const instruction_t& instruction,
                                 const segment_t& segment,
                                 const call_t& call,
                                 processAccess_t accessFunc) {
  // loop over all memory accesses of the instruction
  const AccessTable::insAccessMap_t& insAccessMap = accessTable.getInsAccessMap();
  const auto& avIt = insAccessMap.find(instruction.instruction_id);
  if (avIt != insAccessMap.end()) {
    for (const auto& it : avIt->second) {

      auto accessIt = accessTable.find(it);
      if (accessIt != accessTable.end()) {
        const access_t& access = accessIt->second;

        processAccessGeneric(access.sql_id,
                             access,
                             instruction,
                             segment,
                             call,
                             accessFunc);
      } else {
        BOOST_LOG_TRIVIAL(error) << "Access not found: " << it;
        return IN_NO_ENTRY;
      }
    }
  }
  return IN_OK;
}

int DBInterpreter::processStart() {
  int ret = IN_NO_ENTRY;

  auto callIt = callTable.find(call_t::MAIN);
  if (callIt != callTable.end()) {
    const call_t& call = callIt->second;
    auto threadIt = threadTable.find(call.thread_id);
    if (threadIt != threadTable.end()) {
      const thread_t& thread = threadIt->second;
      processFork(thread);
      processCall(call, static_cast<LIN_NO>(0), static_cast<SEG_ID>(0));
      ret = IN_OK;
    }
  }

  return ret;
}

int DBInterpreter::processEnd() {
  int ret = IN_NO_ENTRY;

  auto callIt = callTable.find(call_t::MAIN);
  if (callIt != callTable.end()) {
    const call_t& call = callIt->second;
    auto threadIt = threadTable.find(call.thread_id);
    if (threadIt != threadTable.end()) {
      const thread_t& thread = threadIt->second;

      // publish a return call event for main
      ShadowThread* sThread = threadMgr_->getThread(call.thread_id);
      ReturnInfo info(call.sql_id, call.end_time);
      ReturnEvent event(sThread, &info);
      _eventService->publish(&event);

      // publish a thread end event for the main thread
      ThreadEndInfo  end_info(call.end_time, thread.id);
      ThreadEndEvent end_event(sThread, &end_info);
      _eventService->publish(&end_event);

      ret = IN_OK;
    }
  }

  return ret;
}

int DBInterpreter::processInstruction(const instruction_t& ins) {
  int ret = IN_NO_ENTRY;

  auto segmentIt = segmentTable.find(ins.segment_id);
  if (segmentIt != segmentTable.end()) {
    const segment_t& segment = segmentIt->second;
    
    auto callIt = callTable.find(segment.call_id);
    if (callIt != callTable.end()) {
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
        ret = processAccess(ins, segment, call, &DBInterpreter::processAcqAccess);
        break;
      case InstructionType::RELEASE:
        ret = processAccess(ins, segment, call, &DBInterpreter::processRelAccess);
        break;
      case InstructionType::FORK:
        for (const auto& it : threadTable) {
          const thread_t& thread = it.second;
          if (ins.instruction_id == thread.create_instruction_id) {
            ret = processFork(thread);
          }
        }
        break;
      case InstructionType::JOIN:
        for (const auto& it : threadTable) {
          const thread_t& thread = it.second;
          if (ins.instruction_id == thread.join_instruction_id) {
              ret = processJoin(ins, segment, call, thread);
          }
        }
        break;
      default:
        ret = IN_NO_ENTRY;
      }
    }
  }

  return ret;
}

size_t DBInterpreter::getHash(unsigned funId, unsigned lineNo) const {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

int DBInterpreter::processCall(const call_t& call, LIN_NO line, SEG_ID segId) {
  int ret = IN_NO_ENTRY;

  auto functionIt = functionTable.find(call.function_id);
  if (functionIt != functionTable.end()) {
    const function_t& function = functionIt->second;

    switch(function.type) {
    case FunctionType::FUNCTION:
    case FunctionType::METHOD:
    {
      // serch for the file where the call has happened from
      auto fileIt = fileTable.find(function.file_id);
      if (fileIt != fileTable.end()) {
        const file_t& file = fileIt->second;

        CallInfo info( static_cast<CALLSITE>(getHash(call.function_id, line)),
                       static_cast<TIME>(call.start_time),
                       static_cast<TIME>(call.end_time - call.start_time),
                       function.signature,
                       segId,
                       function.type,
                       file.file_name,
                       file.file_path);

        ShadowThread* thread = threadMgr_->getThread(call.thread_id);
        CallEvent event(thread, &info);
        _eventService->publish(&event);
        callStack_.push(call.sql_id);
        ret = IN_OK;
        break;
      }
    }
    default:
      break;
    }
  }

  return ret;
}

int DBInterpreter::processCall(const instruction_t& ins) {

  int ret = IN_NO_ENTRY;

  auto callIt = callTable.find(getCallID(ins));
  if (callIt != callTable.end()) {
    const call_t& call = callIt->second;
    ret = processCall(call, ins.line_number, ins.segment_id);
  }

  return ret;
}

int DBInterpreter::processAccessGeneric(ACC_ID accessId,
                                        const access_t& access,
                                        const instruction_t& instruction,
                                        const segment_t& segment,
                                        const call_t& call,
                                        processAccess_t func) {

//  std::string refId = std::string(access.reference_id);
//  auto search = referenceTable.find(_refNoIdMap[refId]);
    REF_ID refId = access.reference_id;
    auto search = referenceTable.find(refId);
    if ( search != referenceTable.end() ) {

        (this->* func)(accessId, access, instruction, segment,
                       call, search->second);

    } else {
        BOOST_LOG_TRIVIAL(error) << "Reference not found: " << access.reference_id;

        return IN_NO_ENTRY;
    }

    return IN_OK;
}

int DBInterpreter::processMemAccess(ACC_ID accessId,
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

    ShadowThread* thread = threadMgr_->getThread(call.thread_id);
    AccessInfo info( access.access_type,
                     var,
                     instruction.instruction_id);
    AccessEvent event( thread, &info );
    _eventService->publish( &event );

    return 0;
}

int DBInterpreter::processAcqAccess(ACC_ID accessId,
                                    const access_t& access,
                                    const instruction_t& instruction,
                                    const segment_t& segment,
                                    const call_t& call,
                                    const reference_t& reference) {

    ShadowThread* thread = threadMgr_->getThread(call.thread_id);
    ShadowLock *lock = lockMgr_->getLock(reference.id);
    AcquireInfo info(lock);
    AcquireEvent event( thread, &info );
    _eventService->publish( &event );
    return 0;
}

int DBInterpreter::processRelAccess(ACC_ID accessId,
                                    const access_t& access,
                                    const instruction_t& instruction,
                                    const segment_t& segment,
                                    const call_t& call,
                                    const reference_t& reference) {

    ShadowThread* thread = threadMgr_->getThread(call.thread_id);
    ShadowLock *lock = lockMgr_->getLock(reference.id);
    ReleaseInfo info(lock);
    ReleaseEvent event( thread, &info );
    _eventService->publish( &event );

    return 0;
}

int DBInterpreter::processFork(const thread_t& thread) {
    ShadowThread *pT = threadMgr_->getThread(thread.parent_thread_id);
    ShadowThread *cT = threadMgr_->getThread(thread.id);
    NewThreadInfo info(cT, pT, thread.start_time, thread.end_time);
    NewThreadEvent event( pT, &info );
    _eventService->publish( &event );
    return 0;
}

int DBInterpreter::processJoin(const instruction_t& instruction,
                               const segment_t& segment,
                               const call_t& call,
                               const thread_t& thread) {

    ShadowThread *pT = threadMgr_->getThread(thread.parent_thread_id);
    ShadowThread *cT = threadMgr_->getThread(thread.id);

    ThreadEndInfo  end_info(timeStringToTime(thread.end_time), thread.id);
    ThreadEndEvent end_event(cT, &end_info);
    _eventService->publish(&end_event);

    JoinInfo info(cT, pT);
    JoinEvent event( pT, &info );
    _eventService->publish( &event );

    return 0;
}

template<typename IdT, typename T>
int DBInterpreter::fillGeneric(const char *sql, sqlite3 **db, DBTable<IdT, T>* table) {
    sqlite3_stmt *sqlstmt = 0;

   /* Execute SQL statement */
   if (sqlite3_prepare_v2(*db, sql, strlen(sql), &sqlstmt, NULL) != SQLITE_OK) {
       BOOST_LOG_TRIVIAL(error) << "Error preparing db: " << sqlite3_errmsg(*db);
       return 1;
   }

   // cycles through the entries till the database is over
   bool reading = true;
   while (reading) {
       switch(sqlite3_step(sqlstmt)) {
       case SQLITE_ROW:
           // process the statement with the function passed as argument
           table->fill(sqlstmt);
           break;
       case SQLITE_DONE:
           reading = false;
           break;
       default:
           BOOST_LOG_TRIVIAL(trace) << "Iterating db failed!";
           return 2;
       }
   }

   return 0;
}
