/*
 * DBInterpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef DBINTERPRETER_H_
#define DBINTERPRETER_H_

// Tables
#include "fwd/Access.h"
#include "fwd/Call.h"
#include "fwd/File.h"
#include "fwd/Function.h"
#include "fwd/Instruction.h"
#include "fwd/Loop.h"
#include "fwd/LoopExecution.h"
#include "fwd/LoopIteration.h"
#include "fwd/Reference.h"
#include "fwd/Segment.h"
#include "fwd/Thread.h"

#include "fwd/EventService.h"
#include "fwd/LockMgr.h"
#include "fwd/ThreadMgr.h"

#include "CallStack.h"

#include "DBTable.h"
#include "ShadowVar.h"
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

#include <map>
#include <memory>
#include <string>

#include "Types.h"

/// Error codes.
enum class ErrorCode {
	OK           = 0, ///< @brief Okay.
	NO_ENTRY     = 1, ///< @brief No entry found.
	ENTRY_EXISTS = 2, ///< @brief Entry already exists.
	ABORT        = 3  ///< @brief Operation aborted.
};

/// Database Interpreter.
class DBInterpreter {
public:
	/// Constructor.
	DBInterpreter(std::string&& logFile, 
				  std::shared_ptr<EventService> service,
				  std::unique_ptr<LockMgr>&&   lockMgr,
				  std::unique_ptr<ThreadMgr>&& threadMgr);

	/// _Deleted_ copy constructor.
	DBInterpreter(const DBInterpreter&)            = delete;
	/// _Deleted_ move constructor.
	DBInterpreter(DBInterpreter&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	DBInterpreter& operator=(const DBInterpreter&) = delete;
	/// _Deleted_ move assignment operator.
	DBInterpreter& operator=(DBInterpreter&&)      = delete;
    /// _Default_ destructor.
    ~DBInterpreter()                               = default;

	/// @brief Processes the database.
	/// @details After importing the database entries, starts looping
	/// over the `InstructionTable_`.
	/// @param DBPath The database to process.
	ErrorCode process(const std::string& DBPath);

	/// Returns a pointer to `EventService_`.
	/// @todo Bad interface design! This pointer shouldn't be accessible outside!
	decltype(auto) getEventService() { return EventService_; };

private:

	// types-------------------------------------------------------------------
	typedef ErrorCode (DBInterpreter::*processAccess_t)(ACC_ID accessID,
												  const access_t& access,
												  const instruction_t& instruction,
												  const segment_t& segment,
												  const call_t& call,
												  const reference_t& reference);

	typedef std::map<REF_ID, ShadowVar*> shadowVarMap_t;

	// members-----------------------------------------------------------------
	/// Map from the Access IDs to the Access-table rows imported from the DB.
	AccessTable        AccessTable_;
	/// Map from the Call IDs to the Call-table rows imported from the DB.
	CallTable          CallTable_;
	/// Map from the File IDs to the File-table rows imported from the DB.
	FileTable          FileTable_;
	/// Map from the Function IDs to the Function-table rows imported from the DB.
	FunctionTable      FunctionTable_;
	/// Map from the Instruction IDs to the Instruction-table rows imported from the DB.
	InstructionTable   InstructionTable_;
	/// Map from the Loop IDs to the Loop-table rows imported from the DB.
	LoopTable          LoopTable_;
	/// Map from the Loop Execution IDs to the Loop-Execution-table rows imported from the DB.
	LoopExecutionTable LoopExecutionTable_;
	/// Map from the Loop Iteration IDs to the Loop-Iteration-table rows imported from the DB.
	LoopIterationTable LoopIterationTable_;
	/// Map from the Reference IDs to the Reference-table rows imported from the DB.
	ReferenceTable     ReferenceTable_;
	/// Map from the Segment IDs to the Segment-table rows imported from the DB.
	SegmentTable       SegmentTable_;
	/// Map from the Thread IDs to the Thread-table rows imported from the DB.
	ThreadTable        ThreadTable_;

	/// Stack of call IDs.
	CallStack CallStack_;
  
	std::shared_ptr<EventService> EventService_;
	shadowVarMap_t _shadowVarMap;

    std::unique_ptr<LockMgr>   lockMgr_;
    std::unique_ptr<ThreadMgr> threadMgr_;

	// private methods---------------------------------------------------------
	static InstructionType transformInstrType(const instruction_t& ins);
	static ReferenceType getVarType(ReferenceType memType);

	/// @brief Imports the database in the tables.
	/// @param DBPath The name of the database to import.
	void importDB(const std::string& DBPath);

	ErrorCode processAccess(const instruction_t& instruction,
			const segment_t& segment,
			const call_t& call,
			processAccess_t accessFunc);


	ErrorCode processReturn(const instruction_t& ins, const call_t& call);
    ErrorCode processStart();
    ErrorCode processEnd();
	ErrorCode processInstruction(const instruction_t& instruction);
	ErrorCode processCall(const instruction_t& instruction);
    ErrorCode processCall(const call_t& call, LIN_NO callLine, SEG_ID segId);
	ErrorCode processAccessGeneric(ACC_ID accessId,
							 const access_t& access,
							 const instruction_t& instruction,
							 const segment_t& segment,
							 const call_t& call,
							 processAccess_t func);
	ErrorCode processMemAccess(ACC_ID accessId,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
    ErrorCode processAcquire(const instruction_t& instruction);
    ErrorCode processRelease(const instruction_t& instruction);
	ErrorCode processJoin(const instruction_t& instruction,
			const segment_t& segment,
			const call_t& call,
			const thread_t& thread);
    ErrorCode processFork(const thread_t& thread);

  /// Parent call ID of an instruction
  /// @return The call ID, if it's found in `segmentT_`
  /// or `IN_NO_ENTRY` otherwise.
  /// @todo Properly implement error handling.
  const CAL_ID getCallerID(const instruction_t& ins) const;
  /// Call ID of a call instruction
  /// @return The call ID, if it's found in `callT_`
  /// or `IN_NO_ENTRY` otherwise.
  /// @todo Properly implement error handling.
  const CAL_ID getCallID(const instruction_t& ins) const;
};


#endif /* DBINTERPRETER_H_ */
