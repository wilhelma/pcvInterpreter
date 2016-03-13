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

/**
 * @bug Should this be a constant?
 */
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

int DBInterpreter::processInstruction(const instruction_t& ins) {

	processAccess_t accessFunc = nullptr;
	segment_t* segment = nullptr;
	call_t* call = nullptr;

	if ( segmentT_.get(ins.segment_id, &segment) == IN_OK) {  
		switch( ins.instruction_type ) {
		case Instruction::CALL:
				processSegment(ins.segment_id, *segment, ins);
			break;
		case Instruction::MEMACCESS:
			if ( callT_.get(segment->call_id, &call) == IN_OK) {
				accessFunc = &DBInterpreter::processMemAccess;
			}	
 			break;
		case Instruction::ACQUIRE:
			if ( callT_.get(segment->call_id, &call) == IN_OK) {
				accessFunc = &DBInterpreter::processAcqAccess;
			}
			break;
		case Instruction::RELEASE:
			if ( callT_.get(segment->call_id, &call) == IN_OK) {
				accessFunc = &DBInterpreter::processRelAccess;
			}
			break;
		case Instruction::FORK:
			{
				thread_t *thread;
				if ( threadT_.get(ins.instruction_id, &thread ) == IN_OK)
					if (callT_.get(segment->call_id, &call) == IN_OK)
					processFork(ins,
								*segment,
								*call,
								*thread);
				break;
			}
		case Instruction::JOIN:
			{
				thread_t *thread;
				if ( threadT_.get(ins.instruction_id, &thread ) == IN_OK)
					if (callT_.get(segment->call_id, &call) == IN_OK)
					processJoin(ins,
								*segment,
								*call,
								*thread);
				break;
			}
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

				processAccessGeneric(search->first,
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

int DBInterpreter::processSegment(unsigned int segmentId,
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

int DBInterpreter::processCall(CAL_ID callId,
							   const call_t& call,
							   const segment_t& seg,
							   const instruction_t& ins) {

	auto search = functionT_.find(call.function_id);
	if (search != functionT_.end()) {

		switch(search->second.type) {
		case Function::FUNCTION:
		case Function::METHOD:
		{
			auto searchFile = fileT_.find(search->second.file_id);
			if (searchFile != fileT_.end()) {
				CallInfo info( call.end_time, // todo: use runtime!
							   search->second.signature,
							   search->second.type,
							   searchFile->second.file_name,
							   searchFile->second.file_path);

				ShadowThread* thread = threadMgr_->getThread(call.thread_id);
				CallEvent event(thread, &info);
				_eventService->publish(&event);
			} else {
				BOOST_LOG_TRIVIAL(error) << "File not found: " << search->second.file_id;
				return 1;
			}
			break;
		}
		default:
			break;
		}
	} else {	 
		BOOST_LOG_TRIVIAL(error) << "Function not found: " << call.function_id;
		return IN_NO_ENTRY;
	}

	return IN_OK;
}

int DBInterpreter::processAccessGeneric(ACC_ID accessId,
										const access_t& access,
										const instruction_t& instruction,
										const segment_t& segment,
										const call_t& call,
										processAccess_t func) {

//	std::string refId = std::string(access.reference_id);
//	auto search = referenceT_.find(_refNoIdMap[refId]);
	int refId = access.reference_id;
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
		

	ShadowThread *pT = threadMgr_->getThread(call.thread_id);
	ShadowThread *cT = threadMgr_->getThread(thread.child_thread_id);
	NewThreadInfo info(cT);					  
	NewThreadEvent event( pT, &info );
	_eventService->publish( &event );

	return 0;
}

int DBInterpreter::processJoin(const instruction_t& instruction,
							   const segment_t& segment,
							   const call_t& call,
							   const thread_t& thread) {
									   
	ShadowThread *pT = threadMgr_->getThread(call.thread_id);
	ShadowThread *cT = threadMgr_->getThread(thread.child_thread_id);
	JoinInfo info(cT);					  
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
	std::cout << "Loading Access" << std::endl;
	if ((rc = fillGeneric("SELECT * from Access;",
						  db, &DBInterpreter::fillAccess)) != 0) return rc;
	// XXX Call worked out
	std::cout << "Loading Call" << std::endl;
	if ((rc = fillGeneric("SELECT * from Call;",
						  db, &DBInterpreter::fillCall)) != 0) return rc;
	std::cout << "Loading File" << std::endl;
	if ((rc = fillGeneric("SELECT * from File;",
						  db, &DBInterpreter::fillFile)) != 0) return rc;
	std::cout << "Loading Function" << std::endl;
	if ((rc = fillGeneric("SELECT * from Function;",
						  db, &DBInterpreter::fillFunction)) != 0) return rc;
	std::cout << "Loading Instruction" << std::endl;
	if ((rc = fillGeneric("SELECT * from Instruction;",
						  db, &DBInterpreter::fillInstruction)) != 0) return rc;
	std::cout << "Loading Loop" << std::endl;
	if ((rc = fillGeneric("SELECT * from Loop;",
						  db, &DBInterpreter::fillLoop)) != 0) return rc;
	std::cout << "Loading LoopExecution" << std::endl;
	if ((rc = fillGeneric("SELECT * from LoopExecution;",
						  db, &DBInterpreter::fillLoopExecution)) != 0) return rc;
	std::cout << "Loading LoopIteration" << std::endl;
	if ((rc = fillGeneric("SELECT * from LoopIteration;",
						  db, &DBInterpreter::fillLoopIteration)) != 0) return rc;
	std::cout << "Loading Reference" << std::endl;
	if ((rc = fillGeneric("SELECT * from Reference;",
						  db, &DBInterpreter::fillReference)) != 0) return rc;
	std::cout << "Loading Segment" << std::endl;
	if ((rc = fillGeneric("SELECT * from Segment;",
						  db, &DBInterpreter::fillSegment)) != 0) return rc;
	std::cout << "Loading Thread" << std::endl;
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
	ACC_ID id             = sqlite3_column_int(sqlstmt, 0);
	INS_ID instruction_id = sqlite3_column_int(sqlstmt, 1);
	int position          = sqlite3_column_int(sqlstmt, 2);
	int reference_id      = sqlite3_column_int(sqlstmt, 3);
	int access_type       = sqlite3_column_int(sqlstmt, 4);
	int memory_state      = sqlite3_column_int(sqlstmt, 5);

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
    int id             = sqlite3_column_int(sqlstmt, 0);
    int thread_id      = sqlite3_column_int(sqlstmt, 1);
    int function_id    = sqlite3_column_int(sqlstmt, 2);
    int instruction_id = sqlite3_column_int(sqlstmt, 3);
    int start_time     = sqlite3_column_int(sqlstmt, 4);
    int end_time       = sqlite3_column_int(sqlstmt, 5);

    std::cout << "reading into call_t\n";
    call_t *tmp = new call_t(id,
			                 thread_id,
							 function_id,
							 instruction_id,
							 start_time,
							 end_time);

    std::cout << "read into call_t\n";
    callT_.fill(id, *tmp);		 
    std::cout << "callT_ filled\n";
    return 0;
}

int DBInterpreter::fillFile(sqlite3_stmt *sqlstmt) {
   int id                         = sqlite3_column_int(sqlstmt, 0);
   const unsigned char *file_path = sqlite3_column_text(sqlstmt, 1);

   file_t *tmp = new file_t(id,
		                    file_path);

   fileT_.fill(id, *tmp);	 
   return 0;
}

int DBInterpreter::fillFunction(sqlite3_stmt *sqlstmt) {
    int id                         = sqlite3_column_int(sqlstmt, 0);
    const unsigned char *signature = sqlite3_column_text(sqlstmt, 1);
    FUN_TYP type                   = sqlite3_column_int(sqlstmt, 2);
    FIL_ID file_id                 = sqlite3_column_int(sqlstmt, 3);
    LIN_NO line_number             = sqlite3_column_int(sqlstmt, 4);

   function_t *tmp = new function_t(id,
		                            signature,
		   	   	   	  	   	   	    type,
		   	   	   	   	   	   	    file_id,
                                    line_number);

   functionT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillInstruction(sqlite3_stmt *sqlstmt) {
   int id               = sqlite3_column_int(sqlstmt, 0);
   int segment_id       = sqlite3_column_int(sqlstmt, 1);
   int instruction_type = sqlite3_column_int(sqlstmt, 2);
   int line_number      = sqlite3_column_int(sqlstmt, 3);

   instruction_t *tmp = new instruction_t(id,
										  segment_id,
										  instruction_type,
										  line_number);

   instructionT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillLoop(sqlite3_stmt *sqlstmt) {
   int id = sqlite3_column_int(sqlstmt, 0);
   int lineNumber = sqlite3_column_int(sqlstmt, 1);

   loop_t *tmp = new loop_t(id,
		                    lineNumber);

   loopT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillLoopExecution(sqlite3_stmt *sqlstmt) {
	int id              = sqlite3_column_int(sqlstmt, 0);
	int loopID          = sqlite3_column_int(sqlstmt, 1);
	int parentIteration = sqlite3_column_int(sqlstmt, 2);
	int loopDuration    = sqlite3_column_int(sqlstmt, 3);

	loopExecution_t *tmp = new loopExecution_t(id,
			                                   loopID,
			                                   parentIteration,
											   loopDuration);

	loopExecutionT_.fill(id, *tmp);
	return 0;
}

int DBInterpreter::fillLoopIteration(sqlite3_stmt *sqlstmt) {
	int id            = sqlite3_column_int(sqlstmt, 0);
	int loopExecution = sqlite3_column_int(sqlstmt, 1);
	int loopIteration = sqlite3_column_int(sqlstmt, 2);

	loopIteration_t *tmp = new loopIteration_t(loopExecution,
											   loopIteration);

	loopIterationT_.fill(id, *tmp);
	return 0;
}

int DBInterpreter::fillReference(sqlite3_stmt *sqlstmt) {
   int id                    = sqlite3_column_int(sqlstmt, 0);
   int size                  = sqlite3_column_int(sqlstmt, 1);
   int memory_type           = sqlite3_column_int(sqlstmt, 2);
   const unsigned char *name = sqlite3_column_text(sqlstmt, 3);
   int allocinstr            = sqlite3_column_int(sqlstmt, 4);

   reference_t *tmp = new reference_t(id,
		   	   	   	   	   	   	   	  size,
		   	   	   	   	   	   	   	  memory_type,
		   	   	   	   	   	   	   	  name,
		   	   	   	   	   	   	   	  allocinstr);

   referenceT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillSegment(sqlite3_stmt *sqlstmt) {
   int id           = sqlite3_column_int(sqlstmt, 0);
   int call_id      = sqlite3_column_int(sqlstmt, 1);
   int segment_type = sqlite3_column_int(sqlstmt, 2);
   int loop_pointer = sqlite3_column_int(sqlstmt, 3);

   segment_t *tmp = new segment_t(call_id,
		   	   	   	   	   	   	  segment_type,
		   	   	   	   	   	   	  loop_pointer);

   segmentT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillThread(sqlite3_stmt *sqlstmt) {
   int id               = sqlite3_column_int(sqlstmt, 0);
   int process_id       = sqlite3_column_int(sqlstmt, 1);
   int instruction_id   = sqlite3_column_int(sqlstmt, 2);
   int child_thread_id  = sqlite3_column_int(sqlstmt, 3);
   int parent_thread_id = sqlite3_column_int(sqlstmt, 4);

   thread_t *tmp = new thread_t(id,
                                process_id,
		   	   	   	   	   	   	instruction_id,
		   	   	   	   	   	   	child_thread_id,
		   	   	   	   	   	   	parent_thread_id);

   threadT_.fill(instruction_id, *tmp);
   return 0;
}
