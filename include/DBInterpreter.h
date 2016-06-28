/*
 * DBInterpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef DBINTERPRETER_H_
#define DBINTERPRETER_H_

#include <sqlite3.h>
#include <map>
#include <vector>
#include <string.h>

#include "CallStack.h"
#include "Interpreter.h"
#include "EventService.h"
#include "ShadowThread.h"
#include "ShadowVar.h"

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
#include "DBTable.h"

#include "AccessTable.h"
#include "CallTable.h"
#include "FileTable.h"
#include "FunctionTable.h"
#include "InstructionTable.h"
#include "LoopTable.h"
#include "LoopExecutionTable.h"
#include "LoopIterationTable.h"
#include "ReferenceTable.h"
#include "SegmentTable.h"
#include "ThreadTable.h"

#include "Types.h"

class LockMgr;
class ThreadMgr;

/******************************************************************************
 * Database Interpreter
 *****************************************************************************/
class DBInterpreter : public Interpreter {
public:
	DBInterpreter(const char* DBPath,
			      const char* logFile, 
				  EventService *service,
				  LockMgr *lockMgr,
				  ThreadMgr *threadMgr)
		: Interpreter(lockMgr, threadMgr, logFile), _dbPath(DBPath),
		  _logFile(logFile), _eventService(service)
	{};

	~DBInterpreter() {};

	virtual int process() override final;
	virtual EventService* getEventService() override final { return _eventService; };

private:

	// types-------------------------------------------------------------------
	typedef int (DBInterpreter::*fillFunc_t)(sqlite3_stmt*);
	typedef int (DBInterpreter::*processAccess_t)(ACC_ID accessID,
												  const access_t& access,
												  const instruction_t& instruction,
												  const segment_t& segment,
												  const call_t& call,
												  const reference_t& reference);

	typedef std::map<REF_ID, ShadowVar*> shadowVarMap_t;

	// members-----------------------------------------------------------------
	AccessTable        accessTable;
	CallTable          callTable;
	FileTable          fileTable;
	FunctionTable      functionTable;
	InstructionTable   instructionTable;
	LoopTable          loopTable;
	LoopExecutionTable loopExecutionTable;
	LoopIterationTable loopIterationTable;
	ReferenceTable     referenceTable;
	SegmentTable       segmentTable; 
	ThreadTable        threadTable;

	CallStack callStack_;
  
	const char* _dbPath;
	const char* _logFile;
	EventService *_eventService;
	shadowVarMap_t _shadowVarMap;

	// private methods---------------------------------------------------------
	static InstructionType transformInstrType(const instruction_t& ins);
	static ReferenceType getVarType(ReferenceType memType);

	int loadDB(const char* path, sqlite3 **db);
	int closeDB(sqlite3 **db);
	int importDataBase(sqlite3 **db);

	template<typename IdT, typename T>
	int fillGeneric(const char *sql, sqlite3 **db, DBTable<IdT, T>* table);

	int processReturn(const instruction_t& ins);

	int processInstruction(const instruction_t& instruction);
	int processSegment(SEG_ID segmentId,
					   const segment_t& segment,
					   const instruction_t& instruction);
	int processCall(CAL_ID callId,
					const call_t& call,
					const segment_t& segment,
					const instruction_t& instruction);
	int processAccessGeneric(ACC_ID accessId,
							 const access_t& access,
							 const instruction_t& instruction,
							 const segment_t& segment,
							 const call_t& call,
							 processAccess_t func);
	int processMemAccess(ACC_ID accessId,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
	int processAcqAccess(ACC_ID accessID,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
	int processRelAccess(ACC_ID accessID,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
	int processJoin(const instruction_t& instruction,
					const segment_t& segment,
					const call_t& call,
					const thread_t& thread);
  int processFork(const instruction_t& instruction,
                  const segment_t& segment,
                  const call_t& call,
                  const thread_t& thread);

  size_t getHash(unsigned funId, unsigned lineNo) const;

  /// Parent call ID of an instruction
  /// @return The call ID, if it's found in `segmentT_`
  /// or `IN_NO_ENTRY` otherwise.
  const CAL_ID getCallerID(const instruction_t& ins) const;
  /// Call ID of a call instruction
  /// @return The call ID, if it's found in `callT_`
  /// or `IN_NO_ENTRY` otherwise.
  const CAL_ID getCallID(const instruction_t& ins) const;

	// prevent generated functions
	DBInterpreter(const DBInterpreter&);
	DBInterpreter& operator=(const DBInterpreter&);
};


#endif /* DBINTERPRETER_H_ */
