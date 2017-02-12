/**
 *  @file  DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "DBInterpreter.h"
#include "SQLException.h"

// -- Database tables ----------
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
// -----------------------------

// -- Database records ---------
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
// -----------------------------

#include "Database.h"

#include "EventGenerator.h"
#include "Types.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <stack>
#include <string>

// logging system
#include "easylogging++.h"

DBInterpreter::DBInterpreter(std::unique_ptr<EventGenerator>&& event_generator) noexcept:
    EventGenerator_(std::move(event_generator)),
    CallStack_(std::make_unique<CallStack>()),
    LastSegmentId_(segment_t::no_id())
{}

// Default the destructor here to allow the Ptr to Impl idiom.
DBInterpreter::~DBInterpreter() = default;

void DBInterpreter::process(const std::string& DBPath) {
    try {
        // fill internal tables with the database entries
        Database_ = load_database(DBPath);
    } catch (const SQLException& e) {
        // this should only happen if the database can't be closed
        LOG(FATAL) << e.what();
        std::abort();
    }

    // handle start of main routine
    processStart();

    // process database entries
    for (const auto& instruction : *database()->instructionTable())
        processInstruction(instruction);

    // handle end of main routine
    processEnd();
}

// Helper function to publish a thread return
void publish_thread_return(const TRD_ID& thread_id,
                           const std::unique_ptr<const Database>& db,
                           const TIME& end_time,
                           const std::unique_ptr<EventGenerator>& event_generator) {
    const auto& thread  = thread_with_id(thread_id, *db); // may throw
    assert(event_generator->lastThreadId() == thread.parent_thread_id);
    event_generator->threadEndEvent(thread.parent_thread_id, thread.id, end_time);
}

void DBInterpreter::processReturn(const instruction_t& ins, const call_t& call) {
    while (!callStack()->empty() && call.id < callStack()->top()) {
        // Retrieve the call at the top of the stack.
        const auto& top_call = call_with_id(callStack()->top(), *database()); // may throw

        // Publish the call return.
        EventGenerator_->returnEvent(top_call.thread_id, top_call.id);
        callStack()->pop(); // TODO Do it in the return event!

        // Check if the threads has changed.
        if (top_call.thread_id != EventGenerator_->lastThreadId())
            publish_thread_return(top_call.thread_id, database(), top_call.end_time, EventGenerator_);
    }

    // The call stack should never be empty.
    assert(!callStack()->empty());
}

void DBInterpreter::processAccess(const instruction_t& instruction, const TRD_ID& thread_id) const {
    // loop over the memory accesses of the instruction
    for (const auto& acc_id : access_ids_of(instruction, *database())) {
        const auto& access = access_with_id(acc_id, *database()); // may throw!
        const auto& ref_of_acc = reference_of(access, *database());
        EventGenerator_->accessEvent(thread_id, ref_of_acc, access.instruction_id, access.access_type);
    }
}

void DBInterpreter::processStart() {
    const auto& main_call        = call_with_id(call_t::main_id(), *database()); // may throw
    const auto& main_call_thread = thread_of(main_call, *database());
    processFork(main_call_thread);
    processCall(main_call, static_cast<LIN_NO>(0), segment_t::main_id());
}

void DBInterpreter::processEnd() {
    // close final calls
    while (!callStack()->empty()) {
        const auto& top_call = call_with_id(callStack()->top(), *database()); // may trow
        callStack()->pop();

        if (top_call.thread_id != EventGenerator_->lastThreadId()) {
            const auto& last_thread = thread_with_id(EventGenerator_->lastThreadId(), *database());
            EventGenerator_->threadEndEvent(top_call.thread_id, last_thread.id, last_thread.start_cycle + last_thread.num_cycles);
        }

        EventGenerator_->returnEvent(top_call.thread_id, top_call.id);
    }
}

void DBInterpreter::processInstruction(const instruction_t& ins) {
    // Segment containing the instruction
    const auto& seg_of_ins  = segment_of(ins, *database());
    // This is now the caller of the instruction
    const auto& caller_of_seg = call_of(seg_of_ins, *database());

    if (seg_of_ins.id != LastSegmentId_) {
        // The segment should always be increasing.
        assert(seg_of_ins.id > LastSegmentId_);

        if (callStack()->top() > caller_of_seg.id) {
            // Make sure that this isn't a call event.
            assert(ins.instruction_type != InstructionType::CALL);
            processReturn(ins, caller_of_seg);
        }
    }


    switch (ins.instruction_type) {
        case InstructionType::CALL:
        {
            // The segment should always be increasing.
            assert(LastSegmentId_ < seg_of_ins.id);

            // Update the last segment ID.
            LastSegmentId_ = seg_of_ins.id;

            return processCall(call_of(ins, *database()), ins.line_number, ins.segment_id);
        }

        case InstructionType::ACCESS:
            return processAccess(ins, caller_of_seg.thread_id); 

        case InstructionType::ACQUIRE:
            return processAcquire(ins, caller_of_seg);

        case InstructionType::RELEASE:
            return processRelease(ins, caller_of_seg);

        case InstructionType::FORK:
            return processFork(thread_forked_by(ins, *database()));

        case InstructionType::JOIN:
            return processJoin(thread_joined_by(ins, *database()));

        default:
            return;
    }
}

// helper function
const CALLSITE call_hash(const FUN_ID& fun_id, const LIN_NO& line) noexcept {
    const auto h1 = std::hash<unsigned>()(fun_id);
    const auto h2 = std::hash<unsigned>()(line);

    return static_cast<CALLSITE>(h1 ^ (h2 << 1));
}

void DBInterpreter::processCall(const call_t& call, const LIN_NO& line, const SEG_ID& seg_id) {
    const auto& fun_of_call = function_of(call, *database());

    switch(fun_of_call.type) {
        case FunctionType::FUNCTION:
        case FunctionType::METHOD:
        {
            EventGenerator_->callEvent(
                    call.thread_id,
                    call.id,
                    call_hash(call.function_id, line),
                    call.start_time,
                    call.end_time,
                    fun_of_call.id,
                    fun_of_call.name,
                    fun_of_call.type,
                    seg_id);
            std::cout << "Call id: " << call.id << std::endl;
            const auto call_id = call.id;
            callStack()->emplace(call_id);
            break;
        }
        default:
            return;
    }
}

/// @todo Code duplication! Candidate for template!
void DBInterpreter::processAcquire(const instruction_t& ins, const call_t& call_of_ins) const {
    const auto& acc_of_ins  = access_of(ins, *database()); // may throw
    EventGenerator_->acquireEvent(call_of_ins.thread_id, acc_of_ins.reference_id, call_of_ins.start_time);
}

/// @todo Code duplication! Candidate for template!
void DBInterpreter::processRelease(const instruction_t& ins, const call_t& call_of_ins) const {
    const auto& acc_of_ins  = access_of(ins, *database()); // may throw
    EventGenerator_->releaseEvent(call_of_ins.thread_id, acc_of_ins.reference_id, call_of_ins.start_time);
}

void DBInterpreter::processFork(const thread_t& new_thread) const noexcept {
    EventGenerator_->newThreadEvent(new_thread.parent_thread_id, new_thread.id, new_thread.start_cycle, new_thread.num_cycles);
}

void DBInterpreter::processJoin(const thread_t& joined_thread) const noexcept {
    const auto end_time = joined_thread.start_cycle + joined_thread.num_cycles;
    EventGenerator_->joinEvent(joined_thread.parent_thread_id, joined_thread.id, end_time);
}
