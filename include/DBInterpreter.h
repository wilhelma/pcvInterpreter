/*
 * DBInterpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef DBINTERPRETER_H_
#define DBINTERPRETER_H_

// -- Database records ---------
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

/// @ingroup database
/// @brief Interprets the content of the `Database` and uses the `EventGenerator` to publish events to the `Tool`'s.
class DBInterpreter {
public:
    /// @brief Constructor.
    /// @param event_generator The event generator to create and publish the events.
    explicit DBInterpreter(std::unique_ptr<EventGenerator>&& event_generator) noexcept;

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
    void process(const std::string& DBPath);

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
    void processAccess(const instruction_t& instruction, const TRD_ID& thread_id) const;


    void processReturn(const instruction_t& ins, const call_t& call);
    void processStart();
    void processEnd();

    /// @brief Calls other _process_ member functions based on the istruction type.
    /// @param instruction The instruction to process.
    void processInstruction(const instruction_t& instruction);

    /// @brief Submits a CallEvent to the EventGenerator.
    /// @param call   The called function.
    /// @param line   The line of the call instruction.
    /// @param seg_id The ID of the segment containing the instruction.
    void processCall(const call_t& call, const LIN_NO& line, const SEG_ID& seg_id);

    /// @brief Submits an AcquireEvent to the EventGenerator.
    /// @param instruction The instruction from which the memory was accessed.
    /// @param caller      The function that called that instruction.
    void processAcquire(const instruction_t& instruction, const call_t& caller) const;

    /// @brief Submits a ReleaseEvent to the EventGenerator.
    /// @param instruction The instruction from which the memory was accessed.
    /// @param caller      The function that called that instruction.
    void processRelease(const instruction_t& instruction, const call_t& caller) const;

    /// @brief Submits a JoinEvent to the EventGenerator.
    /// @param joined_thread    The joined thread.
    void processJoin(const thread_t& joined_thread) const noexcept;

    /// @brief Submits a NewThreadEvent to the EventGenerator.
    /// @param new_thread The forked thread.
    void processFork(const thread_t& new_thread) const noexcept;
};

#endif
