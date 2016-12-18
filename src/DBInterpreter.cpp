/**
 *  @file  DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */
#include "DBInterpreter.h"

#include "SQLException.h"

#include <iostream>
#include <string>
#include <functional>
#include <cassert>

// Database tables
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
#include "DBTable.h"

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

#include "Database.h"

#include "EventGenerator.h"
#include "Types.h"

#include <memory>

// logging system
#include "easylogging++.h"

size_t getHash(unsigned funId, unsigned lineNo) {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

DBInterpreter::DBInterpreter(std::unique_ptr<EventGenerator>&& event_generator) :
    EventGenerator_(std::move(event_generator))
{}

// Default the destructor here to allow the Ptr to Impl idiom.
DBInterpreter::~DBInterpreter() = default;

ErrorCode DBInterpreter::process(const std::string& DBPath) {
    // fill internal tables with the database entries
    try {
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

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processReturn(const instruction_t& ins,
        const call_t& call) {
    TRD_ID returnThread = thread_t::no_id();

    CAL_ID topCallId = CallStack_.top();
    while (!CallStack_.isEmpty() && call.id != topCallId) {
        const auto& topCall = call_with_id(topCallId, *database()); // may throw

        if (returnThread != thread_t::no_id() && returnThread != topCall.thread_id) {
            publishThreadReturn(topCall.thread_id);
            returnThread = thread_t::no_id();
        }

        publishCallReturn(topCall);

        if (call.thread_id != topCall.thread_id)
            returnThread = topCall.thread_id;

        CallStack_.pop();
        topCallId = CallStack_.top();
    }

    if (returnThread != thread_t::no_id())
        publishThreadReturn(returnThread);

    return ErrorCode::OK;
}

// TODO remove this function and call returnEvent directly
ErrorCode DBInterpreter::publishCallReturn(const call_t& topCall) {
    EventGenerator_->returnEvent(topCall.thread_id, topCall.id, topCall.function_id, topCall.end_time);
    return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishThreadReturn(TRD_ID threadId) {
    const auto& thread       = thread_with_id(threadId, *database()); // may throw
    const TIME threadEndTime = thread.start_cycle + thread.num_cycles;
    EventGenerator_->threadEndEvent(thread.parent_thread_id, thread.id, threadEndTime);

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAccess(const instruction_t& instruction,
        const TRD_ID& thread_id) {

    // loop over the memory accesses of the instruction
    for (const auto& acc_id : access_ids_of(instruction, *database())) {
        const auto& access = access_with_id(acc_id, *database()); // may throw!
        const auto& ref_of_acc = reference_of(access, *database());
        EventGenerator_->accessEvent(thread_id, ref_of_acc, access.instruction_id, access.access_type);
    }

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processStart() {
    const auto& main_call        = call_with_id(call_t::MAIN, *database()); // may throw
    const auto& main_call_thread = thread_of(main_call, *database());
    processFork(main_call_thread);
    processCall(main_call, static_cast<LIN_NO>(0), static_cast<SEG_ID>(0));
    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processEnd() {
    auto ret = ErrorCode::NO_ENTRY;

    // close final calls
    while (!CallStack_.isEmpty()) {
        const auto& top_call = call_with_id(CallStack_.pop(), *database()); // may trow

        if (top_call.thread_id != EventGenerator_->lastThreadId()) {
            const auto& last_thread = thread_with_id(EventGenerator_->lastThreadId(), *database());
            EventGenerator_->threadEndEvent(top_call.thread_id, last_thread.id, last_thread.start_cycle + last_thread.num_cycles);
        }

        EventGenerator_->returnEvent(top_call.thread_id, top_call.id, top_call.function_id, top_call.end_time);

        ret = ErrorCode::OK;
    }

    return ret;
}

ErrorCode DBInterpreter::processInstruction(const instruction_t& ins) {
    // Segment containing the instruction
    const auto& seg_of_ins  = segment_of(ins, *database());
    const auto& call_of_seg = call_of(seg_of_ins, *database());

    // handle call/thread stack returns
    processReturn(ins, call_of_seg);

    switch (ins.instruction_type) {
        case InstructionType::CALL:
            // TODO Replace with a direct call (the line below doesn't work)
            // return processCall(call_of_seg, ins.line_number, ins.segment_id);
            return processCall(ins);

        case InstructionType::ACCESS:
            return processAccess(ins, call_of_seg.thread_id); 

        case InstructionType::ACQUIRE:
            return processAcquire(ins);

        case InstructionType::RELEASE:
            return processRelease(ins);

        case InstructionType::FORK:
            for (const auto& trd: *database()->threadTable())
                if (ins.id == trd.create_instruction_id)
                    return processFork(trd);
            break;

        case InstructionType::JOIN:
            for (const auto& trd : *database()->threadTable())
                if (ins.id == trd.join_instruction_id)
                    return processJoin(ins, trd);
            break;

        default:
            return ErrorCode::NO_ENTRY;
    }

    return ErrorCode::NO_ENTRY;
}

ErrorCode DBInterpreter::processCall(const call_t& call, LIN_NO line, SEG_ID segId) {
    auto ret = ErrorCode::NO_ENTRY;

    const auto& fun_of_call = function_of(call, *database());

    switch(fun_of_call.type) {
        case FunctionType::FUNCTION:
        case FunctionType::METHOD:
        {
            EventGenerator_->callEvent(
                    call.thread_id,
                    call.id,
                    static_cast<CALLSITE>(getHash(call.function_id, line)),
                    call.start_time,
                    static_cast<TIME>(call.end_time - call.start_time),
                    fun_of_call.name,
                    segId,
                    fun_of_call.type);
            CallStack_.push(call.id);
            ret = ErrorCode::OK;
            break;
        }
        default:
            break;
    }

    return ret;
}

ErrorCode DBInterpreter::processCall(const instruction_t& ins) {
    return processCall(call_of(ins, *database()), ins.line_number, ins.segment_id);
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processAcquire(const instruction_t& ins) {
    // This will throw or abort in case of failure
    const auto& call_of_ins = call_of(ins, *database());

    for (const auto& access : *database()->accessTable()) {
        if (access.instruction_id == ins.id) {
            EventGenerator_->acquireEvent(call_of_ins.thread_id, access.reference_id, call_of_ins.start_time);
            return ErrorCode::OK;
        }
    }

    return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processRelease(const instruction_t& ins) {
    // This will throw or abort in case of failure
    const auto& call_of_ins = call_of(ins, *database());

    for (const auto& access : *database()->accessTable()) {
        if (access.instruction_id == ins.id) {
            EventGenerator_->releaseEvent(call_of_ins.thread_id, access.reference_id, call_of_ins.start_time);
            return ErrorCode::OK;
        }
    }

    return ErrorCode::NO_ENTRY;
}

/// TODO Code duplication! Candidate for template!
/// TODO Remove function. Call directly.
ErrorCode DBInterpreter::processFork(const thread_t& thread) {

    // XXX Consider changing prototype to newThreadEvent(const thread_t&)
    EventGenerator_->newThreadEvent(thread.parent_thread_id, thread.id, thread.start_cycle, thread.num_cycles);

    return ErrorCode::OK;
}

/// TODO Code duplication! Candidate for template!
/// TODO Remove function. Call directly.
ErrorCode DBInterpreter::processJoin(const instruction_t& ins,
        const thread_t& thread) {

    EventGenerator_->joinEvent(thread.parent_thread_id, thread.id, EventGenerator_->lastEventTime());

    return ErrorCode::OK;
}
