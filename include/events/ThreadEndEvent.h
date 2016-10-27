/**
 *
 *    @file  ThreadEndEvent.h
 *   @brief  
 *
 *    @date  06/29/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  THREAD_END_EVENT_H_
#define  THREAD_END_EVENT_H_

#include "fwd/ShadowThread.h"
#include "fwd/ThreadEndInfo.h"

#include "Event.h"

#include <memory>

/// @ingroup events
/// @brief Event for a thread end (which happens together or before a join).
class ThreadEndEvent final : public Event<ThreadEndInfo> {
public:
    /// @brief Constructor.
    /// @param  thread The thread the event was triggered from.
    /// @tparam info   The thread end event information.
    explicit ThreadEndEvent(std::shared_ptr<const ShadowThread> thread,
                            std::unique_ptr<const ThreadEndInfo>&& info)
        : Event(thread, std::move(info))
    {};

    /// Returns the event type.
    Events type() const final
    { return Events::THREADEND; };
};

#endif
