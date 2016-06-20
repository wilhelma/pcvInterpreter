/**
 *  @file  DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */
#include "DBInterpreter.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include "Event.h"
#include "EventService.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "LockMgr.h"
#include "ThreadMgr.h"
#include "DBTable.h"

DBInterpreter::DBInterpreter(const char* DBPath,
                             const char* logFile,
                             EventService *service,
                             LockMgr *lockMgr,
                             ThreadMgr *threadMgr) 
    : Interpreter(lockMgr, threadMgr, logFile), _dbPath(DBPath), _logFile(logFile),
      _eventService(service) { }

DBInterpreter::~DBInterpreter(){ }

EventService* DBInterpreter::getEventService() {
    return _eventService;
}

int DBInterpreter::loadDB(const char* path, sqlite3 **db) {

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

int DBInterpreter::closeDB(sqlite3 **db) {

    if ( sqlite3_close(*db) != SQLITE_OK)
        BOOST_LOG_TRIVIAL(fatal) <<  "Can't close database - error: "  
                                 <<  sqlite3_errmsg(*db);

    return IN_OK;
}

int DBInterpreter::process() {

    sqlite3 *db;

    // open the database
    if ( loadDB(_dbPath, &db) != IN_OK ) {
        return IN_ABORT;
    }

    // fill the internal maps with database entries
    int rc = fillStructures(&db);
    if ( rc != IN_OK) {
        BOOST_LOG_TRIVIAL(error) << "Can't fill internal structures."
                                 << " Error code: " << rc;
        return IN_ABORT;
    }

    // process database entries
    for (auto instruction : instructionT_)
        processInstruction(instruction.second);

    closeDB(&db);
    return 0;
}


const CAL_ID DBInterpreter::getCallerID(const instruction_t& ins) const {
	auto search = segmentT_.find(ins.segment_id);
	if (search == segmentT_.end()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id
			<< " not found in segmentT_ table";
		return static_cast<CAL_ID>(IN_NO_ENTRY);
	}
	return search->second.call_id;
}

const CAL_ID DBInterpreter::getCallID(const instruction_t& ins) const {
	// look for the call id of the instruction
	CAL_ID call_id = static_cast<CAL_ID>(0);
	bool found = false;
	for (auto& it : callT_) {
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
	auto search = segmentT_.find(ins.segment_id);
	if (search == segmentT_.end()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id << "not found";
		return IN_NO_ENTRY;
	}

	if (!callStack_.isEmpty()) {
		CAL_ID parent_call_id = search->second.call_id;
		while (parent_call_id != callStack_.top()) {
			callStack_.pop();

			// in this case, the call didn't happen in the
			// parent ID scope, so the "parent" call has returned
			auto parent_call = callT_.find(parent_call_id);
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
    segment_t* segment         = nullptr;
    call_t* call               = nullptr;

	// search for `ins.segment_id` in the `segmentT_` table and put
	// its pointer into `segment`
	if (segmentT_.get(ins.segment_id, &segment) == IN_OK) {  
		// if the segment is found, check the instruction type
		switch (ins.instruction_type) {
			case InstructionType::CALL:
				// XXX this doesn't change `accessFunc`
				processSegment(ins.segment_id, *segment, ins);
				processReturn(ins);
				break;
			case InstructionType::ACCESS:
				if (callT_.get(segment->call_id, &call) == IN_OK)
					accessFunc = &DBInterpreter::processMemAccess;
				break;
			case InstructionType::ALLOC:
				if ( callT_.get(segment->call_id, &call) == IN_OK)
					accessFunc = &DBInterpreter::processAcqAccess;
				break;
			case InstructionType::FREE:
				if ( callT_.get(segment->call_id, &call) == IN_OK)
					accessFunc = &DBInterpreter::processRelAccess;
				break;
			case InstructionType::FORK:
				for (auto it : threadT_) {
					thread_t& thread = it.second;
					if (ins.instruction_id == thread.create_instruction_id)
						if (callT_.get(segment->call_id, &call) == IN_OK)
							processFork(ins, *segment, *call, thread);
				}
				break;
			case InstructionType::JOIN:
				for (auto it : threadT_) {
					thread_t& thread = it.second;
					if (ins.instruction_id == thread.join_instruction_id)
						if (callT_.get(segment->call_id, &call) == IN_OK)
							processJoin(ins, *segment, *call, thread);
				}
				break;
			default:
				return IN_NO_ENTRY;
		}   
	}            

	if (accessFunc != nullptr) {
		auto search = _insAccessMap.find(ins.instruction_id);
		accessVector_t::iterator it;
		for (it = search->second.begin(); it != search->second.end(); it++) {

			auto searchAccess = accessT_.find(*it);
			if (searchAccess != accessT_.end()) {

				processAccessGeneric((ACC_ID) search->first,
						searchAccess->second,
						ins,
						*segment,
						*call,
						accessFunc);
			} else {
				BOOST_LOG_TRIVIAL(error) << "Access not found: " << *it;
				return IN_NO_ENTRY;
			}
		}
	}

	return IN_OK;
}

int DBInterpreter::processSegment(SEG_ID segmentId,
                                  const segment_t& seg,
                                  const instruction_t& ins) {

    auto search = callT_.find(seg.call_id);
    if (search != callT_.end()) {
        if (!processCall(seg.call_id, search->second, seg, ins))
            return 1;
    } else {
        BOOST_LOG_TRIVIAL(error) << "Call not found: " << seg.call_id;
        return 1;
    }

    return 0;
}

size_t DBInterpreter::getHash(unsigned funId, unsigned lineNo) const {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

int DBInterpreter::processCall(CAL_ID callId,
                               const call_t& call,
                               const segment_t& seg,
                               const instruction_t& ins) {

	// fetch called function 
	auto search = functionT_.find(call.function_id);

	// check that the function exists
	if (search == functionT_.end()) {
		BOOST_LOG_TRIVIAL(error) << "Function not found: " << call.function_id;
		return IN_NO_ENTRY;
	}

	switch(search->second.type) {
		case FunctionType::FUNCTION:
		case FunctionType::METHOD:
			{
				// serch for the file where the call has happened from
				auto searchFile = fileT_.find(search->second.file_id);
				if (searchFile == fileT_.end()) {
					BOOST_LOG_TRIVIAL(error) << "File not found: " << search->second.file_id;
					return 1;
				}

				CallInfo info( (CALLSITE)getHash(call.function_id, ins.line_number),
						(TIME) (call.end_time - call.start_time),
						(FUN_SG)search->second.signature,
						(SEG_ID) call.sql_id, // XXX BUG segment or call?
						search->second.type,
						(FIL_PT)searchFile->second.file_name,
						(FIL_PT)searchFile->second.file_path);

				ShadowThread* thread = threadMgr_->getThread(call.thread_id);
				CallEvent event(thread, &info);
				_eventService->publish(&event);
				break;
			}
		default:
			break;
	}

	return IN_OK;
}

int DBInterpreter::processAccessGeneric(ACC_ID accessId,
                                        const access_t& access,
                                        const instruction_t& instruction,
                                        const segment_t& segment,
                                        const call_t& call,
                                        processAccess_t func) {

//  std::string refId = std::string(access.reference_id);
//  auto search = referenceT_.find(_refNoIdMap[refId]);
    REF_ID refId = access.reference_id;
    auto search = referenceT_.find(refId);
    if ( search != referenceT_.end() ) {

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
    NewThreadInfo info(cT, pT, thread.num_cycles, thread.start_time);
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
    JoinInfo info(cT, pT);
    JoinEvent event( pT, &info );
    _eventService->publish( &event );

    return 0;
}

/**
 * Fills the `db` database.
 * 
 * For each type of SQL statement, a different funcion is called as
 * `fillGeneric()` argument to fill `db` (e.g. `fillAccess()` for the `Access` table).
 *
 */
int DBInterpreter::fillStructures(sqlite3 **db) {

    int rc;

    // Read from all the tables in the database
    if ((rc = fillGeneric("SELECT * from Access;",
                          db, &DBInterpreter::fillAccess)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Call;",
                          db, &DBInterpreter::fillCall)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from File;",
                          db, &DBInterpreter::fillFile)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Function;",
                          db, &DBInterpreter::fillFunction)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Instruction;",
                          db, &DBInterpreter::fillInstruction)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Loop;",
                          db, &DBInterpreter::fillLoop)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from LoopExecution;",
                          db, &DBInterpreter::fillLoopExecution)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from LoopIteration;",
                          db, &DBInterpreter::fillLoopIteration)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Reference;",
                          db, &DBInterpreter::fillReference)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Segment;",
                          db, &DBInterpreter::fillSegment)) != 0) return rc;
    if ((rc = fillGeneric("SELECT * from Thread;",
                          db, &DBInterpreter::fillThread)) != 0) return rc;

    BOOST_LOG_TRIVIAL(trace) << "Rows in Access:        " << accessT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Call:          " << callT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in File:          " << fileT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Function:      " << functionT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Instruction:   " << instructionT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Loop:          " << instructionT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in LoopExecution: " << instructionT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in LoopIteration: " << instructionT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Reference:     " << referenceT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Segment:       " << segmentT_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Thread:        " << threadT_.size();
    return 0;
}

/**
 * Executes the statement `sql` on the database `db` and processes the output with `func`. 
 *
 * @param sql String cointaining the SQL statement (e.g. "SELECT * from THREAD_TABLE;")
 * @param db The database object
 * @param func Pointer to a `DBInterpreter::` method used to process the statement (e.g. `fillThread()`)
 *
 * @return 2 in case of error, 0 in case of success.
 */
int DBInterpreter::fillGeneric(const char *sql, sqlite3 **db, fillFunc_t func) {
    /**
     * __About Statements__ Think of each SQL statement as a separate computer program.
     * The original SQL text is source code. A prepared statement object is the compiled
     * object code. All SQL must be converted into a prepared statement before it can be run.
     */
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
           (this->* func)(sqlstmt);
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

int DBInterpreter::fillAccess(sqlite3_stmt *sqlstmt) {
    ACC_ID id                = (ACC_ID)sqlite3_column_int(sqlstmt, 0);
    INS_ID instruction_id    = (INS_ID)sqlite3_column_int(sqlstmt, 1);
    POS position             = (POS)sqlite3_column_int(sqlstmt, 2);
    REF_ID reference_id      = (REF_ID)sqlite3_column_int(sqlstmt, 3);
    AccessType access_type   = (AccessType)sqlite3_column_int(sqlstmt, 4);
    AccessState memory_state = (AccessState)sqlite3_column_int(sqlstmt, 5);

    access_t *tmp = new access_t(id,
                                 instruction_id,
                                 position,
                                 reference_id,
                                 access_type,
                                 memory_state); 

    accessT_.fill(id, *tmp);         
    _insAccessMap[instruction_id].push_back(id); // create 1:n associations 
    return 0;
}

int DBInterpreter::fillCall(sqlite3_stmt *sqlstmt) {
    CAL_ID id             = (CAL_ID)sqlite3_column_int(sqlstmt, 0);
    TRD_ID thread_id      = (TRD_ID)sqlite3_column_int(sqlstmt, 1);
    FUN_ID function_id    = (FUN_ID)sqlite3_column_int(sqlstmt, 2);
    INS_ID instruction_id = (INS_ID)sqlite3_column_int(sqlstmt, 3);
    TIME start_time       = (TIME)sqlite3_column_int(sqlstmt, 4);
    TIME end_time         = (TIME)sqlite3_column_int(sqlstmt, 5);

    call_t *tmp = new call_t(id,
                             thread_id,
                             function_id,
                             instruction_id,
                             start_time,
                             end_time);

    callT_.fill(id, *tmp);       
    return 0;
}

int DBInterpreter::fillFile(sqlite3_stmt *sqlstmt) {
   FIL_ID id        = (FIL_ID)sqlite3_column_int(sqlstmt, 0);
   FIL_PT file_path((const char*)sqlite3_column_text(sqlstmt, 1));

   file_t *tmp = new file_t(id,
                            file_path);

   fileT_.fill(id, *tmp);    
   return 0;
}

int DBInterpreter::fillFunction(sqlite3_stmt *sqlstmt) {
    FUN_ID id          = (FUN_ID)sqlite3_column_int(sqlstmt, 0);
    FUN_SG signature((const char*)sqlite3_column_text(sqlstmt, 2));
    FIL_ID file_id     = (FIL_ID)sqlite3_column_int(sqlstmt, 3);
    LIN_NO line_number = (LIN_NO)sqlite3_column_int(sqlstmt, 4);
    FunctionType type  = (FunctionType)sqlite3_column_int(sqlstmt, 6);

   function_t *tmp = new function_t(id,
                                    signature,
                                    type,
                                    file_id,
                                    line_number);

   functionT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillInstruction(sqlite3_stmt *sqlstmt) {
   INS_ID id                        = (INS_ID)sqlite3_column_int(sqlstmt, 0);
   SEG_ID segment_id                = (SEG_ID)sqlite3_column_int(sqlstmt, 1);
   InstructionType instruction_type = (InstructionType)sqlite3_column_int(sqlstmt, 2);
   LIN_NO line_number               = (LIN_NO)sqlite3_column_int(sqlstmt, 3);

   instruction_t *tmp = new instruction_t(id,
                                          segment_id,
                                          instruction_type,
                                          line_number);

   instructionT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillLoop(sqlite3_stmt *sqlstmt) {
   LOP_ID id         = (LOP_ID)sqlite3_column_int(sqlstmt, 0);
   LIN_NO lineNumber = (LIN_NO)sqlite3_column_int(sqlstmt, 1);

   loop_t *tmp = new loop_t(id,
                            lineNumber);

   loopT_.fill(id, *tmp);
   return 0;
}

// TODO
/// @todo
int DBInterpreter::fillLoopExecution(sqlite3_stmt *sqlstmt) {
    LOE_ID id              = (LOE_ID)sqlite3_column_int(sqlstmt, 0);
    LOE_ID loopID          = (LOE_ID)sqlite3_column_int(sqlstmt, 1);
    int parentIteration = sqlite3_column_int(sqlstmt, 2);
    int loopDuration    = sqlite3_column_int(sqlstmt, 3);

    loopExecution_t *tmp = new loopExecution_t(id,
                                               loopID,
                                               parentIteration,
                                               loopDuration);

    loopExecutionT_.fill(id, *tmp);
    return 0;
}

//TODO
/// @todo
int DBInterpreter::fillLoopIteration(sqlite3_stmt *sqlstmt) {
    LOI_ID id            = (LOI_ID)sqlite3_column_int(sqlstmt, 0);
    LOE_ID loopExecution = (LOE_ID)sqlite3_column_int(sqlstmt, 1);
    LOI_ID loopIteration = (LOI_ID)sqlite3_column_int(sqlstmt, 2);

    loopIteration_t *tmp = new loopIteration_t(id,
                                               loopExecution,
                                               loopIteration);

    loopIterationT_.fill(id, *tmp);
    return 0;
}

int DBInterpreter::fillReference(sqlite3_stmt *sqlstmt) {
    REF_ID id                 = (REF_ID)sqlite3_column_int(sqlstmt, 0);
    REF_SIZE size             = (REF_SIZE)sqlite3_column_int(sqlstmt, 1);
    ReferenceType memory_type = (ReferenceType)sqlite3_column_int(sqlstmt, 2);
    INS_ID allocinstr         = (INS_ID)sqlite3_column_int(sqlstmt, 4);
    REF_NAME name((const char*)sqlite3_column_text(sqlstmt, 3));

    reference_t *tmp = new reference_t(id,
            size,
            memory_type,
            name,
            allocinstr);

    referenceT_.fill(id, *tmp);
    return 0;
}

int DBInterpreter::fillSegment(sqlite3_stmt *sqlstmt) {
   SEG_ID id                = (SEG_ID)sqlite3_column_int(sqlstmt, 0);
   CAL_ID call_id           = (CAL_ID)sqlite3_column_int(sqlstmt, 1);
   SegmentType segment_type = (SegmentType)sqlite3_column_int(sqlstmt, 2);
   LOI_ID loop_pointer      = (LOI_ID)sqlite3_column_int(sqlstmt, 3);      // XXX check type

   segment_t *tmp = new segment_t(id,
                                  call_id,
                                  segment_type,
                                  loop_pointer);

   segmentT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillThread(sqlite3_stmt *sqlstmt) {
   TRD_ID id               = (TRD_ID)sqlite3_column_int(sqlstmt, 0);
   TIME_STRING start_time  = TIME_STRING(reinterpret_cast<const char*>(
         sqlite3_column_text(sqlstmt, 1)));
   TIME_STRING end_time		 = TIME_STRING(reinterpret_cast<const char*>(
         sqlite3_column_text(sqlstmt, 2)));
   NUM_CYCLES num_cycles	 = (NUM_CYCLES)sqlite3_column_int(sqlstmt, 3);
   INS_ID create_instr_id  = (INS_ID)sqlite3_column_int(sqlstmt, 4);
   INS_ID join_instr_id    = (INS_ID)sqlite3_column_int(sqlstmt, 5);
   TRD_ID parent_thread_id = (TRD_ID)sqlite3_column_int(sqlstmt, 6);
   PID process_id          = (PID)sqlite3_column_int(sqlstmt, 7);

   thread_t *tmp = new thread_t(id,
                                start_time,
                                end_time,
                                num_cycles,
                                create_instr_id,
                                join_instr_id,
                                parent_thread_id,
                                process_id);

   threadT_.fill(id, *tmp);
   return 0;
}
