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

#include "LockMgr.h"
#include "ThreadMgr.h"

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

DBInterpreter::DBInterpreter(std::unique_ptr<EventService> service,
        std::unique_ptr<LockMgr>&&   lockMgr,
        std::unique_ptr<ThreadMgr>&& threadMgr) :
    lastEventTime_(0),
    EventService_(std::move(service)),
    lockMgr_(std::move(lockMgr)),
    threadMgr_(std::move(threadMgr))
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
    for (const auto& instruction : database()->instructionTable())
        processInstruction(instruction.second);

    // handle end of main routine
    processEnd();

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processReturn(const instruction_t& ins,
        const call_t& call) {
    TRD_ID returnThread = NO_TRD_ID;

    CAL_ID topCallId =CallStack_.top();
    while (!CallStack_.isEmpty() && call.id != topCallId) {
        auto callIt = database()->callTable().find(topCallId);
        if (callIt != database()->callTable().end()) {
            const call_t& topCall = callIt->second;

            if (returnThread != NO_TRD_ID && returnThread != topCall.thread_id) {
                publishThreadReturn(topCall.thread_id);
                returnThread = NO_TRD_ID;
            }

            publishCallReturn(topCall);

            if (call.thread_id != topCall.thread_id)
                returnThread = topCall.thread_id;

            CallStack_.pop();
            topCallId = CallStack_.top();
        } else {
            return ErrorCode::NO_ENTRY;
        }
    }

    if (returnThread != NO_TRD_ID)
        publishThreadReturn(returnThread);

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishCallReturn(const call_t& topCall) {
    assert(lastEventTime_ <= topCall.end_time);

    auto sThread = getThread(topCall.thread_id);
    ReturnInfo info(topCall.id, topCall.function_id, topCall.end_time);
    ReturnEvent event(sThread, &info);
    eventService()->publish(&event);
    lastEventTime_ = topCall.end_time;

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::publishThreadReturn(TRD_ID threadId) {
    auto threadIt = database()->threadTable().find(threadId);
    if (threadIt != database()->threadTable().end()) {
        const thread_t& thread = threadIt->second;
        const TIME threadEndTime = thread.start_cycle + thread.num_cycles;
        assert(lastEventTime_ <= threadEndTime);
        ThreadEndInfo  end_info(threadEndTime, threadId);
        auto sThread = getThread(threadId);
        ThreadEndEvent end_event(sThread, &end_info);
        eventService()->publish(&end_event);
        lastEventTime_ = threadEndTime;
        lastThreadId = thread.parent_thread_id;
    } else {
        return ErrorCode::NO_ENTRY;
    }

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAccess(const instruction_t& instruction,
        const segment_t& segment,
        const call_t& call,
        processAccess_t accessFunc) {
    // loop over all memory accesses of the instruction
    const AccessTable::insAccessMap_t& insAccessMap = database()->accessTable().getInsAccessMap();
    const auto& avIt = insAccessMap.find(instruction.id);
    if (avIt != insAccessMap.end()) {
        for (const auto& it : avIt->second) {
            auto accessIt = database()->accessTable().find(it);
            if (accessIt != database()->accessTable().end()) {
                const access_t& access = accessIt->second;
                processAccessGeneric(access.id,
                        access,
                        instruction,
                        segment,
                        call,
                        accessFunc);
            } else {
                LOG(ERROR) << "Access not found: " << it;
                return ErrorCode::NO_ENTRY;
            }
        }
    }
    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processStart() {
    auto ret = ErrorCode::NO_ENTRY;

    auto callIt = database()->callTable().find(call_t::MAIN);
    if (callIt != database()->callTable().end()) {
        const call_t& call = callIt->second;
        auto threadIt = database()->threadTable().find(call.thread_id);
        if (threadIt != database()->threadTable().end()) {
            const thread_t& thread = threadIt->second;
            processFork(thread);
            processCall(call, static_cast<LIN_NO>(0), static_cast<SEG_ID>(0));
            ret = ErrorCode::OK;
        }
    }

    return ret;
}

ErrorCode DBInterpreter::processEnd() {
    auto ret = ErrorCode::NO_ENTRY;

    // close final calls
    while (!CallStack_.isEmpty()) {
        const CAL_ID topCallId = CallStack_.pop();
        auto callIt = database()->callTable().find(topCallId);
        if (callIt != database()->callTable().end()) {
            const call_t& call = callIt->second;
            auto sThread = getThread(call.thread_id);

            if (call.thread_id != lastThreadId) {
                // publish a thread end event
                auto threadIt = database()->threadTable().find(lastThreadId);
                if (threadIt != database()->threadTable().end()) {
                    const thread_t& thread = threadIt->second;
                    ThreadEndInfo end_info(static_cast<TIME>(
                                thread.start_cycle + thread.num_cycles),
                            thread.id);
                    ThreadEndEvent end_event(sThread, &end_info);
                    eventService()->publish(&end_event);
                    lastThreadId = call.thread_id;
                }
            }

            ReturnInfo info (call.id, call.function_id, call.end_time);
            ReturnEvent event(sThread, &info);
            eventService()->publish(&event);
            ret = ErrorCode::OK;
        }
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
            for (const auto& it : database()->threadTable()) {
                const thread_t& thread = it.second;
                if (ins.id == thread.create_instruction_id)
                    return processFork(thread);
            }
            break;

        case InstructionType::JOIN:
            for (const auto& it : database()->threadTable()) {
                const thread_t& thread = it.second;
                if (ins.id == thread.join_instruction_id)
                    return processJoin(ins, thread);
            }
            break;

        default:
            return ErrorCode::NO_ENTRY;
    }

    return ErrorCode::NO_ENTRY;
}

//int DBInterpreter::processSegment(SEG_ID segmentId,
//                                  const segment_t& seg,
//                                  const instruction_t& ins) {
//
//    auto search = database()->callTable().find(seg.call_id);
//    if (search != database()->callTable().end()) {
//        if (!processCall(seg.call_id, search->second, seg, ins))
//            return 1;
//    } else {
//        LOG(ERROR) << "Call not found: " << seg.call_id;
//        return 1;
//    }
//
//    return 0;
//}

ErrorCode DBInterpreter::processCall(const call_t& call, LIN_NO line, SEG_ID segId) {
    assert(lastEventTime_ <= call.start_time);

    auto ret = ErrorCode::NO_ENTRY;

    const auto& fun_of_call = function_of(call, *database());

    switch(fun_of_call.type) {
        case FunctionType::FUNCTION:
        case FunctionType::METHOD:
        {
            const auto& file_of_fun = file_of(fun_of_call, *database());
            CallInfo info(static_cast<CALLSITE>(getHash(call.function_id, line)),
                    call.start_time,
                    static_cast<TIME>(call.end_time - call.start_time),
                    fun_of_call.name,
                    segId,
                    fun_of_call.type,
                    file_of_fun.file_name,
                    file_of_fun.file_path);

            auto thread = getThread(call.thread_id);
            CallEvent event(thread, &info);
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
    // This will throw or abort in case of failure
    return processCall(call_of(ins, *database()), ins.line_number, ins.segment_id);
}

ErrorCode DBInterpreter::processAccessGeneric(ACC_ID accessId,
        const access_t& access,
        const instruction_t& instruction,
        const segment_t& segment,
        const call_t& call,
        processAccess_t func) {

    const auto& ref_of_acc = reference_of(access, *database());
    return (this->* func)(accessId, access, instruction, segment,
            call, ref_of_acc);
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

    auto thread = getThread(call.thread_id);
    AccessInfo info( access.access_type,
            var,
            instruction.id);
    AccessEvent event( thread, &info );
    eventService()->publish( &event );

    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processAcquire(const instruction_t& ins) {
    // This will throw or abort in case of failure
    const auto& call_of_ins = call_of(ins, *database());

    assert(lastEventTime_ <= call_of_ins.start_time);
    for (const auto& accIt : database()->accessTable()) {
        const auto& access = accIt.second;
        if (access.instruction_id == ins.id) {
            const auto& ref_of_acc = reference_of(access, *database());
            const auto& lock = getLock(ref_of_acc.id);
            if (lock != nullptr) {
                const auto& thread = getThread(call_of_ins.thread_id);
                AcquireInfo info(lock, call_of_ins.start_time);
                AcquireEvent event(thread, &info);
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
    for (auto accIt : database()->accessTable()) {
        const access_t& access = accIt.second;
        if (access.instruction_id == ins.id) {
            const auto& ref_of_acc = reference_of(access, *database());
            const auto& lock = getLock(ref_of_acc.id);
            if (lock != nullptr) {
                auto thread = getThread(call_of_ins.thread_id);
                ReleaseInfo info(lock, call_of_ins.start_time);
                ReleaseEvent event( thread, &info );
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
    auto pT = getThread(thread.parent_thread_id);
    auto cT = getThread(thread.id);
    NewThreadInfo info(cT, pT, thread.start_cycle, thread.num_cycles);
    NewThreadEvent event( pT, &info );
    eventService()->publish( &event );
    lastEventTime_ = thread.start_cycle;
    lastThreadId = thread.id;
    return ErrorCode::OK;
}

/// @todo Code duplication! Candidate for template!
ErrorCode DBInterpreter::processJoin(const instruction_t& ins,
        const thread_t& thread) {

    auto pT = getThread(thread.parent_thread_id);
    auto cT = getThread(thread.id);
    JoinInfo info(cT, pT, lastEventTime_);
    JoinEvent event( pT, &info );
    eventService()->publish( &event );
    lastThreadId = thread.parent_thread_id;

    return ErrorCode::OK;
}
