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
#include "Instruction.h" // < Contains InstructionType!
#include "fwd/Loop.h"
#include "fwd/LoopExecution.h"
#include "fwd/LoopIteration.h"
#include "fwd/Reference.h"
#include "fwd/Segment.h"
#include "Thread.h" // < Contains NO_TRD_ID static variable (move to source!)
 
#include "fwd/EventService.h"
#include "fwd/LockMgr.h"
#include "fwd/ThreadMgr.h"

#include "CallStack.h"

#include "fwd/Database.h"

#include "ShadowVar.h"

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

/// @ingroup database
/// @brief Database interpreter: produces Event's from the database types (e.g. access_t, call_t and so on).
/// @details Processes the database rows turning them into Event's.
/// To `publish()` them, the Event's are then passed to the EventService.
class DBInterpreter {
public:
    /// Constructor.
    explicit DBInterpreter(std::unique_ptr<EventService> service,
                           std::unique_ptr<LockMgr>&&    lockMgr,
                           std::unique_ptr<ThreadMgr>&&  threadMgr);

    /// _Deleted_ copy constructor.
    DBInterpreter(const DBInterpreter&)            = delete;
    /// _Deleted_ move constructor.
    DBInterpreter(DBInterpreter&&)                 = delete;
    /// _Deleted_ copy assignment operator.
    DBInterpreter& operator=(const DBInterpreter&) = delete;
    /// _Deleted_ move assignment operator.
    DBInterpreter& operator=(DBInterpreter&&)      = delete;
    /// @brief _Default_ destructor.
    /// @attention It's defaulted in the source to allow the _pointer
    /// to implementation_ idiom with `unique_ptr`s.
    ~DBInterpreter();

    /// @brief Processes the database.
    /// @details After importing the database entries, starts looping
    /// over the `InstructionTable_`.
    /// @param DBPath The database to process.
    ErrorCode process(const std::string& DBPath);

    /// Returns a pointer to `EventService_`.
    /// @todo Bad interface design! This pointer shouldn't be accessible outside!
    const std::unique_ptr<const EventService>& eventService() const noexcept
    { return EventService_; };

    /// Helper function to access the database in a read-only way.
    const std::unique_ptr<const Database>& database() const noexcept
    { return Database_; }

private:

    // types-------------------------------------------------------------------
    typedef ErrorCode (DBInterpreter::*processAccess_t)(ACC_ID accessID,
                                                  const access_t& access,
                                                  const instruction_t& instruction,
                                                  const segment_t& segment,
                                                  const call_t& call,
                                                  const reference_t& reference);

    typedef std::map<REF_ID, ShadowVar*> shadowVarMap_t;
    shadowVarMap_t _shadowVarMap;

    /// Content of the input database.
    std::unique_ptr<const Database> Database_;

    /// Stack of call IDs.
    CallStack CallStack_;

    TIME lastEventTime_;
    TRD_ID lastThreadId = NO_TRD_ID;
  
    /// Constant pointer to the EventService.
    const std::unique_ptr<const EventService> EventService_;


    const std::unique_ptr<LockMgr>   LockMgr_;

    /// @brief Maps a thread ID to its ShadowThread.
    const std::unique_ptr<ThreadMgr> ThreadMgr_;

    // private methods---------------------------------------------------------
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
                          const thread_t& thread);
    ErrorCode processFork(const thread_t& thread);

    ErrorCode publishCallReturn(const call_t& topCall);
    ErrorCode publishThreadReturn(TRD_ID threadId);
};

#endif /* DBINTERPRETER_H_ */
