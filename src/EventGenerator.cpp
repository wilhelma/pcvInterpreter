/**
 *
 *    @file  EventGenerator.cpp
 *   @brief  
 *
 *    @date  11/23/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "EventGenerator.h"

#include "Reference.h"

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

#include "EventService.h"
#include "ShadowLock.h"
#include "ShadowLockMap.h"
#include "ShadowThread.h"
#include "ShadowThreadMap.h"
#include "ShadowVariable.h"
#include "ShadowVariableMap.h"

#include "Thread.h"
#include "Types.h"

#include <cassert>
#include <memory>

EventGenerator::EventGenerator(std::unique_ptr<const EventService>&& event_service) noexcept :
    EventService_(std::move(event_service)),
    ShadowLockMap_(std::make_unique<ShadowLockMap>()),
    ShadowThreadMap_(std::make_unique<ShadowThreadMap>()),
    ShadowVariableMap_(std::make_unique<ShadowVariableMap>()),
    LastThreadId_(thread_t::no_id()),
    LastEventTime_(0)
{}

EventGenerator::~EventGenerator() = default;

// Helper function. Get the iterator of the id and, if it's not
// in the map, abort the application.
template <typename SMap>
typename SMap::const_iterator get_iterator(typename SMap::key_type const& id,
                                           SMap const& m) noexcept
{
    auto const& it = m.find(id);
    assert(it != std::cend(m));
    return it;
}

void EventGenerator::accessEvent(const TRD_ID& parent_thread_id,
                                 const reference_t& var_reference,
                                 const INS_ID& instruction_id,
                                 const AccessType access_type)
{
    // Get the shadow for the parent thread
    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);

    // Entry shouldn't already be in the map
    assert(ShadowVariableMap_->find(var_reference.id) == std::cend(*ShadowVariableMap_));

    auto&& sv               = std::make_shared<const ShadowVariable>(var_reference);
    const auto& var_info_it = ShadowVariableMap_->insert(std::make_pair(var_reference.id, std::move(sv))).first;
    auto&& access_info = std::make_unique<const AccessInfo>(var_info_it, instruction_id, access_type);

    AccessEvent access_event(parent_thread_it, std::move(access_info));
    EventService_->publish(&access_event);

    // Remove entry from the map after the use
    ShadowVariableMap_->erase(var_info_it);
}

void EventGenerator::acquireEvent(const TRD_ID& parent_thread_id,
                                  const REF_ID& lock_id,
                                  const TIME& acquire_time) 
{
    assert(LastEventTime_ <= acquire_time);

    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    auto&& acquire_info = acquireInfo(lock_id, acquire_time);
    AcquireEvent acquire_event(parent_thread_it, std::move(acquire_info));
    EventService_->publish(&acquire_event);

    LastEventTime_ = acquire_time;
}

std::unique_ptr<const AcquireInfo> EventGenerator::acquireInfo(const REF_ID& lock_id, const TIME& acquire_time)
{
    // Entry shouldn't already be in the map
    assert(ShadowLockMap_->find(lock_id) == std::cend(*ShadowLockMap_));

    auto lock_entry = std::make_shared<const ShadowLock>(lock_id, acquire_time);
    const auto lock_it = ShadowLockMap_->insert(std::make_pair(lock_id, lock_entry)).first;

    return std::make_unique<const AcquireInfo>(lock_it);
}

void EventGenerator::releaseEvent(const TRD_ID& parent_thread_id,
                                  const REF_ID& lock_id,
                                  const TIME&   release_time)
{
    assert(LastEventTime_ <= release_time);

    const auto& lock_it = get_iterator(lock_id, *ShadowLockMap_);
    auto&& release_info = std::make_unique<const ReleaseInfo>(lock_it, release_time);

    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    ReleaseEvent release_event(parent_thread_it, std::move(release_info));
    EventService_->publish(&release_event);

    // remove information from the table
    ShadowLockMap_->erase(lock_it);

    LastEventTime_ = release_time;
}

void EventGenerator::newThreadEvent(const TRD_ID& parent_thread_id,
                                    const TRD_ID& child_thread_id,
                                    const TIME& start_time,
                                    const TIME& run_time)
{
    assert (LastEventTime_ <= start_time);

    // Make sure the parent is in the map
    ShadowThreadMap::const_iterator parent_thread_it;
    if (ShadowThreadMap_->empty()) {
        auto thread_entry = std::make_shared<const ShadowThread>(start_time);
        parent_thread_it = ShadowThreadMap_->insert(std::make_pair(parent_thread_id, thread_entry)).first;
    } else {
        parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    }

    // TODO change the value of the NO_THREAD variable
//    assert(parent_thread_id != child_thread_id);

    auto&& new_thread_info = newThreadInfo(child_thread_id, start_time, run_time);
    NewThreadEvent new_thread_event(parent_thread_it, std::move(new_thread_info));
    EventService_->publish(&new_thread_event);

    LastEventTime_ = start_time;
    LastThreadId_  = child_thread_id;

}

std::unique_ptr<const NewThreadInfo> EventGenerator::newThreadInfo(const TRD_ID& child_thread_id,
                                                                   const TIME& start_time,
                                                                   const TIME& run_time)
{

    // Make sure the child is not in the map
    // TODO change the value of the NO_THREAD_ID variable
//    assert(ShadowThreadMap_->find(child_thread_id) == std::cend(*ShadowThreadMap_));
    auto thread_entry = std::make_shared<const ShadowThread>(start_time);
    const auto& thread_it = ShadowThreadMap_->insert(std::make_pair(child_thread_id, thread_entry)).first;

    return std::make_unique<const NewThreadInfo>(thread_it, run_time);
}

void EventGenerator::threadEndEvent(const TRD_ID& parent_thread_id,
                                    const TRD_ID& child_thread_id,
                                    const TIME& end_time)
{
    assert(LastEventTime_ <= end_time);

    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    const auto& child_thread_it = get_iterator(child_thread_id, *ShadowThreadMap_);
    auto&& thread_end_info = std::make_unique<const ThreadEndInfo>(child_thread_it, end_time);
    ThreadEndEvent thread_end_event(parent_thread_it, std::move(thread_end_info));
    EventService_->publish(&thread_end_event);

    LastEventTime_ = end_time;
    LastThreadId_  = parent_thread_id;
}

void EventGenerator::joinEvent(const TRD_ID& parent_thread_id,
                               const TRD_ID& child_thread_id,
                               const TIME& join_time)
{
    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    const auto& child_thread_it  = get_iterator(child_thread_id, *ShadowThreadMap_);
    auto&& join_info = std::make_unique<const JoinInfo>(child_thread_it, join_time);
    JoinEvent join_event(parent_thread_it, std::move(join_info));
    EventService_->publish(&join_event);

    // Erase thread from the map
    ShadowThreadMap_->erase(child_thread_it);

    LastThreadId_ = parent_thread_id;
}

void EventGenerator::callEvent(const TRD_ID& parent_thread_id,
                               const CALLSITE& SiteId,
                               const TIME& CallTime,
                               const TIME& Runtime,
                               const FUN_SG& FnSignature,
                               const SEG_ID& Segment,
                               FunctionType FnType,
                               const FIL_PT& FileName,
                               const FIL_PT& FilePath)
{
    assert(LastEventTime_ <= CallTime);

    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    auto&& call_info = std::make_unique<const CallInfo>(SiteId, CallTime, Runtime, FnSignature, Segment, FnType, FileName, FilePath);
    CallEvent call_event(parent_thread_it, std::move(call_info));
    EventService_->publish(&call_event);

    LastEventTime_ = CallTime;
}

void EventGenerator::returnEvent(const TRD_ID& parent_thread_id,
                                 const CAL_ID& call,
                                 const FUN_ID function,
                                 const TIME& return_time)
{
    assert(LastEventTime_ <= return_time);

    const auto& parent_thread_it = get_iterator(parent_thread_id, *ShadowThreadMap_);
    auto&& return_info = std::make_unique<const ReturnInfo>(call, function, return_time);
    ReturnEvent return_event(parent_thread_it, std::move(return_info));
    EventService_->publish(&return_event);

    LastEventTime_ = return_time;
}
