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

#include "SQLException.h"

#include <iostream>
#include <string>
#include <functional>
#include <cassert>

#include "ShadowLockMap.h"
#include "ShadowThreadMap.h"

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

#include "EventService.h"
#include "Types.h"

#include <memory>

// logging system
#include "easylogging++.h"

size_t getHash(unsigned funId, unsigned lineNo) {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

DBInterpreter::DBInterpreter(std::unique_ptr<EventService>&& service,
        std::unique_ptr<ShadowLockMap>&&   lockMgr,
        std::unique_ptr<ShadowThreadMap>&& threadMgr) :
    lastEventTime_(0),
    EventService_(std::move(service)),
    ShadowLockMap_(std::move(lockMgr)),
    ShadowThreadMap_(std::move(threadMgr))
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
    TRD_ID returnThread = NO_TRD_ID;

    CAL_ID topCallId = CallStack_.top();
    while (!CallStack_.isEmpty() && call.id != topCallId) {
        const auto& topCall = call_with_id(topCallId, *database()); // may throw

        if (returnThread != NO_TRD_ID && returnThread != topCall.thread_id) {
            publishThreadReturn(topCall.thread_id);
            returnThread = NO_TRD_ID;
        }

        publishCallReturn(topCall);

        if (call.thread_id != topCall.thread_id)
            returnThread = topCall.thread_id;

        CallStack_.pop();
        topCallId = CallStack_.top();
    }

    if (returnThread != NO_TRD_ID)
        publishThreadReturn(returnThread);

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishCallReturn(const call_t& topCall) {
    assert(lastEventTime_ <= topCall.end_time);

    ReturnEvent event(ShadowThreadMap_->getShadow(topCall.thread_id),
                      std::make_unique<const ReturnInfo>(topCall.id, topCall.function_id, topCall.end_time));
    eventService()->publish(&event);
    lastEventTime_ = topCall.end_time;

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishThreadReturn(TRD_ID threadId) {
    const auto& thread = thread_with_id(threadId, *database()); // may throw
    const TIME threadEndTime = thread.start_cycle + thread.num_cycles;
    assert(lastEventTime_ <= threadEndTime);

    ThreadEndEvent end_event(ShadowThreadMap_->getShadow(threadId),
                             std::make_unique<const ThreadEndInfo>(threadEndTime, threadId));
    eventService()->publish(&end_event);

    lastEventTime_ = threadEndTime;
    lastThreadId = thread.parent_thread_id;

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAccess(const instruction_t& instruction,
        const segment_t& segment,
        const call_t& call,
        processAccess_t accessFunc) {

    // loop over the memory accesses of the instruction
    for (const auto& acc_id : access_ids_of(instruction, *database())) {
        const auto& access = access_with_id(acc_id, *database()); // may throw!
        processAccessGeneric(access.id, access, instruction, segment, call, accessFunc);
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
        const auto& sThread  = ShadowThreadMap_->getShadow(top_call.thread_id);

        if (top_call.thread_id != lastThreadId) {
            const auto& last_thread = thread_with_id(lastThreadId, *database());

            ThreadEndEvent end_event(sThread,
                                     std::make_unique<const ThreadEndInfo>(last_thread.start_cycle + last_thread.num_cycles, last_thread.id));
            eventService()->publish(&end_event);
            lastThreadId = top_call.thread_id;
        }

        ReturnEvent event(sThread,
                          std::make_unique<const ReturnInfo>(top_call.id, top_call.function_id, top_call.end_time));
        eventService()->publish(&event);

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
            return processCall(ins);

        case InstructionType::ACCESS:
            return processAccess(ins, seg_of_ins, call_of_seg, &DBInterpreter::processMemAccess);

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
    assert(lastEventTime_ <= call.start_time);

    auto ret = ErrorCode::NO_ENTRY;

    const auto& fun_of_call = function_of(call, *database());

    switch(fun_of_call.type) {
        case FunctionType::FUNCTION:
        case FunctionType::METHOD:
        {
            const auto& file_of_fun = file_of(fun_of_call, *database());
            auto info = std::make_unique<const CallInfo>(
                    static_cast<CALLSITE>(getHash(call.function_id, line)),
                    call.start_time,
                    static_cast<TIME>(call.end_time - call.start_time),
                    fun_of_call.name,
                    segId,
                    fun_of_call.type,
                    file_of_fun.file_name,
                    file_of_fun.file_path);

            CallEvent event(ShadowThreadMap_->getShadow(call.thread_id),
                            std::move(info));
            eventService()->publish(&event);
            lastEventTime_ = call.start_time;
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

ErrorCode DBInterpreter::processAccessGeneric(ACC_ID accessId,
        const access_t& access,
        const instruction_t& instruction,
        const segment_t& segment,
        const call_t& call,
        processAccess_t func) {

    const auto& ref_of_acc = reference_of(access, *database());
    return (this->* func)(accessId, access, instruction, segment, call, ref_of_acc);
}

ErrorCode DBInterpreter::processMemAccess(ACC_ID accessId,
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

    AccessEvent event(std::make_unique<const ShadowThread>(call.thread_id),
                      std::make_unique<const AccessInfo>(access.access_type, var, instruction.id));
    eventService()->publish(&event);

    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processAcquire(const instruction_t& ins) {
    // This will throw or abort in case of failure
    const auto& call_of_ins = call_of(ins, *database());

    assert(lastEventTime_ <= call_of_ins.start_time);
    for (const auto& access : *database()->accessTable()) {
        if (access.instruction_id == ins.id) {
            const auto& ref_of_acc = reference_of(access, *database());
            const auto& lock = ShadowLockMap_->getShadow(ref_of_acc.id);
            if (lock != nullptr) {
                AcquireEvent event(ShadowThreadMap_->getShadow(call_of_ins.thread_id),
                                   std::make_unique<const AcquireInfo>(lock, call_of_ins.start_time));
                eventService()->publish(&event);

                lastEventTime_ = call_of_ins.start_time;
                return ErrorCode::OK;
            }
        }
    }

    return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processRelease(const instruction_t& ins) {
    // This will throw or abort in case of failure
    const auto& call_of_ins = call_of(ins, *database());

    assert(lastEventTime_ <= call_of_ins.start_time);
    for (const auto& access : *database()->accessTable()) {
        if (access.instruction_id == ins.id) {
            const auto& ref_of_acc = reference_of(access, *database());
            const auto& lock = ShadowLockMap_->getShadow(ref_of_acc.id);
            if (lock != nullptr) {
                ReleaseEvent event(ShadowThreadMap_->getShadow(call_of_ins.thread_id),
                                   std::make_unique<const ReleaseInfo>(lock, call_of_ins.start_time));
                eventService()->publish( &event );
                lastEventTime_ = call_of_ins.start_time;
                return ErrorCode::OK;
            }
        }
    }

    return ErrorCode::NO_ENTRY;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processFork(const thread_t& thread) {
    assert (lastEventTime_ <= thread.start_cycle);
    auto pT = ShadowThreadMap_->getShadow(thread.parent_thread_id);
    auto cT = ShadowThreadMap_->getShadow(thread.id);

    NewThreadEvent event(pT,
                         std::make_unique<const NewThreadInfo>(cT, pT, thread.start_cycle, thread.num_cycles));
    eventService()->publish( &event );

    lastEventTime_ = thread.start_cycle;
    lastThreadId = thread.id;
    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processJoin(const instruction_t& ins,
        const thread_t& thread) {

    auto pT = ShadowThreadMap_->getShadow(thread.parent_thread_id);
    auto cT = ShadowThreadMap_->getShadow(thread.id);

    JoinEvent event(pT,
                    std::make_unique<const JoinInfo>(cT, pT, lastEventTime_));
    eventService()->publish( &event );

    lastThreadId = thread.parent_thread_id;

    return ErrorCode::OK;
}
