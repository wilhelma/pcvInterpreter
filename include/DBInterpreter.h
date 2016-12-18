/*
 * DBInterpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef DBINTERPRETER_H_
#define DBINTERPRETER_H_

// -- Tables -------------------
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
// -----------------------------
 
#include "CallStack.h"

#include "fwd/Database.h"
#include "fwd/EventGenerator.h"

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
/// @brief Interprets the content of the `Database` and uses the `EventGenerator` to publish events to the `Tool`'s.
class DBInterpreter {
public:
    /// @brief Constructor.
    /// @param e_generator The event generator to create and publish the events.
    explicit DBInterpreter(std::unique_ptr<EventGenerator>&& event_generator);

    /// _Deleted_ copy constructor.
    DBInterpreter(const DBInterpreter&) = delete;
    /// _Deleted_ move constructor.
    DBInterpreter(DBInterpreter&&)      = delete;
    /// _Deleted_ copy assignment operator.
    DBInterpreter& operator=(const DBInterpreter&) = delete;
    /// _Deleted_ move assignment operator.
    DBInterpreter& operator=(DBInterpreter&&)      = delete;

    /// @brief _Default_ destructor.
    ~DBInterpreter();

    /// @brief Processes the database by looping over the instruction table.
    /// @param DBPath The database to process.
    ErrorCode process(const std::string& DBPath);

private:
    /// Helper function to access the database in a read-only way.
    const std::unique_ptr<const Database>& database() const noexcept
    { return Database_; }

    /// Constant pointer to the EventGenerator.
    const std::unique_ptr<EventGenerator> EventGenerator_;

    /// Content of the input database.
    std::unique_ptr<const Database> Database_;

    /// Stack of call IDs.
    CallStack CallStack_;

    // private methods---------------------------------------------------------
    ErrorCode processAccess(const instruction_t& instruction, const TRD_ID& thread_id);


    ErrorCode processReturn(const instruction_t& ins, const call_t& call);
    ErrorCode processStart();
    ErrorCode processEnd();
    ErrorCode processInstruction(const instruction_t& instruction);
    ErrorCode processCall(const instruction_t& instruction);
    ErrorCode processCall(const call_t& call, LIN_NO callLine, SEG_ID segId);
    ErrorCode processAcquire(const instruction_t& instruction);
    ErrorCode processRelease(const instruction_t& instruction);
    ErrorCode processJoin(const instruction_t& instruction,
                          const thread_t& thread);
    ErrorCode processFork(const thread_t& thread);

    ErrorCode publishCallReturn(const call_t& topCall);
    ErrorCode publishThreadReturn(TRD_ID threadId);
};

#endif /* DBINTERPRETER_H_ */
