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

    // process database entries
    for (const auto& instruction : instructionTable)
        processInstruction(instruction.second);

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

int DBInterpreter::processReturn(const instruction_t& ins) {

	auto search = segmentTable.find(ins.segment_id);
	if (search == segmentTable.end()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id << "not found";
		return IN_NO_ENTRY;
	}

	if (!callStack_.isEmpty()) {
		CAL_ID parent_call_id = search->second.call_id;
		while (parent_call_id != callStack_.top()) {
			callStack_.pop();

			// in this case, the call didn't happen in the
			// parent ID scope, so the "parent" call has returned
			auto parent_call = callTable.find(parent_call_id);
			ReturnInfo info(parent_call_id, parent_call->second.end_time);
			ShadowThread* thread = threadMgr_->getThread(parent_call->second.thread_id);
			ReturnEvent event(thread, &info);
			_eventService->publish(&event);
		}
	}

	callStack_.push(getCallID(ins));
	return IN_OK;
}

int DBInterpreter::processInstruction(const instruction_t& ins) {
    processAccess_t accessFunc = nullptr;

	SegmentTable::const_iterator search_segment = segmentTable.find(ins.segment_id);
	CallTable::const_iterator search_call = callTable.cend(); // dumb value
	if (search_segment != segmentTable.end()) {
		switch (ins.instruction_type) {
			case InstructionType::CALL:
        printf("CALL instruction \n");
        processCall(ins);
        processReturn(ins);
				break;
			case InstructionType::ACCESS:
        printf("ACCESS instruction \n");
				search_call = callTable.find(search_segment->second.call_id);
				if (search_call != callTable.cend())
					accessFunc = &DBInterpreter::processMemAccess;
				break;
      case InstructionType::ACQUIRE:
				search_call = callTable.find(search_segment->second.call_id);
				if (search_call != callTable.cend())	
          accessFunc = &DBInterpreter::processAcqAccess;
				break;
      case InstructionType::RELEASE:
				search_call = callTable.find(search_segment->second.call_id);
				if (search_call != callTable.cend())
					accessFunc = &DBInterpreter::processRelAccess;
				break;
			case InstructionType::FORK:
				for (const auto& it : threadTable) {
					const thread_t& thread = it.second;
					if (ins.instruction_id == thread.create_instruction_id) {
						search_call = callTable.find(search_segment->second.call_id);
						if (search_call != callTable.cend())
							processFork(ins, search_segment->second, search_call->second, thread);
					}
				}
				break;
			case InstructionType::JOIN:
				for (const auto& it : threadTable) {
					const thread_t& thread = it.second;
					if (ins.instruction_id == thread.join_instruction_id) {
						search_call = callTable.find(search_segment->second.call_id);
						if (search_call != callTable.cend())
							processJoin(ins, search_segment->second, search_call->second, thread);
					}
				}
				break;
			default:
				return IN_NO_ENTRY;
		}
	}

	if (accessFunc != nullptr) {
		// loop over all memory accesses of the instruction
    const AccessTable::insAccessMap_t& insAccessMap = accessTable.getInsAccessMap();
    const auto& avIt = insAccessMap.find(ins.instruction_id);
    if (avIt != insAccessMap.end()) {
      for (const auto& it : avIt->second) {

        auto searchAccess = accessTable.find(it);
        if (searchAccess != accessTable.end()) {

          // possible BUG (conversion INS_ID -> ACC_ID)
          processAccessGeneric(searchAccess->first,
              searchAccess->second,
              ins,
              search_segment->second,
              search_call->second,
              accessFunc);
        } else {
          BOOST_LOG_TRIVIAL(error) << "Access not found: " << it;
          return IN_NO_ENTRY;
        }
      }
    }
	}

	return IN_OK;
}

size_t DBInterpreter::getHash(unsigned funId, unsigned lineNo) const {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}


int DBInterpreter::processCall(const instruction_t& instruction) {

  int ret = IN_NO_ENTRY;

  auto callIt = callTable.find(getCallID(instruction));
  if (callIt != callTable.end()) {
    const call_t& call = callIt->second;


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

          CallInfo info( static_cast<CALLSITE>(getHash(call.function_id, instruction.line_number)),
                         static_cast<TIME>(call.end_time - call.start_time),
                         function.signature,
                         instruction.segment_id,
                         function.type,
                         file.file_name,
                         file.file_path);

          ShadowThread* thread = threadMgr_->getThread(call.thread_id);
          CallEvent event(thread, &info);
          _eventService->publish(&event);
          ret = IN_OK;
          break;
        }
      }
      default:
          break;
      }
    }
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

int DBInterpreter::processFork(const instruction_t& instruction,
                               const segment_t& segment,
                               const call_t& call,
                               const thread_t& thread) {

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
