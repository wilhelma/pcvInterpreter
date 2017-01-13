/**
 *
 *    @file  EventGenerator.h
 *   @brief  
 *
 *    @date  11/23/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  EVENT_GENERATOR_H_
#define  EVENT_GENERATOR_H_

#include "fwd/EventService.h"
#include "fwd/ShadowCallMap.h"
#include "fwd/ShadowLockMap.h"
#include "fwd/ShadowThreadMap.h"
#include "fwd/ShadowVariableMap.h"

// Records --------------------
#include "fwd/Reference.h"
#include "fwd/Function.h"
// ----------------------------

// Infos ----------------------
#include "fwd/AcquireInfo.h"
#include "fwd/CallInfo.h"
#include "fwd/NewThreadInfo.h"
// ----------------------------

#include "fwd/AccessType.h"
#include "Types.h"

#include <memory>

/// @ingroup events
/// @brief Generates events and delivers them to the EventService.
/// @details This classe moves logic away from the DBInterpreter and
/// reduces keeps memory consumption under control as it also manages
/// the entries in the shadow maps.
class EventGenerator {
public:
    /// @brief Constructor.
    /// @param event_service The event service to publish events to the tools.
    explicit EventGenerator(std::unique_ptr<const EventService>&& event_service) noexcept;

    /// _Default_ destructor.
    ~EventGenerator();

    /// _Deleted_ copy constructor.
    EventGenerator(const EventGenerator&)            = delete;
    /// _Deleted_ copy assignment operator.
    EventGenerator& operator=(const EventGenerator&) = delete;

    /// _Deleted_ move constructor.
    EventGenerator(EventGenerator&&)            = delete;
    /// _Deleted_ move assignment operator.
    EventGenerator& operator=(EventGenerator&&) = delete;

    /// @brief Creates and delivers an AccessEvent to the event service.
    /// @param parent_thread_id The ID of the parent thread.
    /// @param var_reference    The information about the accessed variable.
    /// @param instruction_id   The ID of the instruction where the variable was accessed from.
    /// @param access_type      The access type.
    void accessEvent(const TRD_ID& parent_thread_id,
                     const reference_t& var_reference,
                     const INS_ID& instruction_id,
                     const AccessType access_type);
    
    /// @brief Creates and delivers an AcquireEvent to the event service.
    /// @param parent_thread_id The ID of the parent thread.
    /// @param lock_id          The ID of the acquired lock.
    /// @param acquire_time     The time when it was acquired.
    void acquireEvent(const TRD_ID& parent_thread_id,
                      const REF_ID& lock_id,
                      const TIME& acquire_time);

    /// @brief Creates and delivers a ReleaseEvent to the event service.
    /// @param parent_thread_id The ID of the parent thread.
    /// @param lock_id          The ID of the released lock.
    /// @param release_time     The time when it was released.
    void releaseEvent(const TRD_ID& parent_thread_id,
                      const REF_ID& lock_id,
                      const TIME& release_time);

    /// @brief Creates and delivers a NewThreadEvent to the event service.
    /// @param parent_thread_id The ID of the parent thread.
    /// @param child_thread_id  The ID of the created thread
    /// @param start_time       The time when the thread was created.
    /// @param run_time         The runtime of the child thread.
    void newThreadEvent(const TRD_ID& parent_thread_id,
                        const TRD_ID& child_thread_id,
                        const TIME& start_time,
                        const TIME& run_time);

    /// @brief Creates and delivers a ThreadEndEvent to the event service.
    /// @param parent_thread_id The ID of the parent thread.
    /// @param child_thread_id  The ID of the created thread
    /// @param end_time         The time when the child thread has finished its work.
    void threadEndEvent(const TRD_ID& parent_thread_id,
                        const TRD_ID& child_thread_id,
                        const TIME& end_time);

    /// @brief Creates and delivers a JoinEvent to the event service.
    /// @param parent_thread_id The ID of the parent thread.
    /// @param child_thread_id  The ID of the created thread
    /// @param join_time        The time when the child thread was joined.
    void joinEvent(const TRD_ID& parent_thread_id,
                   const TRD_ID& child_thread_id,
                   const TIME& join_time);

    /// Creates and delivers a CallEvent to the event service.
    void callEvent(const TRD_ID& parent_thread_id,
                   const CAL_ID& call_id,
                   const CALLSITE& site_id,
                   const TIME& call_time,
                   const TIME& return_time,
                   const FUN_ID& function_id,
                   const FUN_SG& function_signature,
                   FunctionType function_type,
                   const SEG_ID& segment_id);

    /// Creates and delivers a ReturnEvent to the event service.
    /// @param return_time The time when the call returns.
    void returnEvent(const TRD_ID& parent_thread_id,
                     const CAL_ID& call_id);

    /// Returns the last (called?, created?) thread ID.
    const TRD_ID& lastThreadId() const noexcept
    { return LastThreadId_; }

    const TIME& lastEventTime() const noexcept
    { return LastEventTime_; }
private:
    /// Constant pointer to the EventService.
    const std::unique_ptr<const EventService> EventService_;
    /// Maps a call ID to its ShadowCall.
    const std::unique_ptr<ShadowCallMap>      ShadowCallMap_;
    /// Maps a reference ID to its ShadowLock.
    const std::unique_ptr<ShadowLockMap>      ShadowLockMap_;
    /// Maps a thread ID to its ShadowThread.
    const std::unique_ptr<ShadowThreadMap>    ShadowThreadMap_;
    /// Maps a reference ID to its ShadowVariable.
    const std::unique_ptr<ShadowVariableMap>  ShadowVariableMap_;

    /// The ID of the thread at the top of the stack.
    TRD_ID LastThreadId_;
    /// The time when last event happened.
    TIME   LastEventTime_;

    /// @brief Creates a new lock-acquire information class.
    /// @param lock_id      The ID of the acquired lock.
    /// @param acquire_time The time when it was acquired.
    std::unique_ptr<const AcquireInfo> acquireInfo(const REF_ID& lock_id,
                                                   const TIME& acquire_time);

    /// @brief Creates an event for a new thread creation.
    /// @param child_thread_id The ID of the created thread
    /// @param start_time      The time when the thread was created.
    /// @param run_time        The runtime of the child thread.
    std::unique_ptr<const NewThreadInfo> newThreadInfo(const TRD_ID& child_thread_id,
                                                       const TIME& start_time,
                                                       const TIME& run_time);

    std::unique_ptr<const CallInfo> callInfo(const CAL_ID& call_id,
                                             const CALLSITE& site_id,
                                             const TIME& call_time,
                                             const TIME& runtime,
                                             const FUN_ID& function_id,
                                             const FUN_SG& function_signature,
                                             FunctionType function_type,
                                             const SEG_ID& segment_id);
};

#endif
