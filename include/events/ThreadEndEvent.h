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

#include "fwd/EventType.h"
#include "fwd/ShadowThread.h"
#include "fwd/ThreadEndInfo.h"

#include "Event.h"
#include "Types.h"

#include <memory>

/// @ingroup events
/// @brief Event for a thread end (which happens together or before a join).
class ThreadEndEvent final : public Event<ThreadEndInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The thread end event information.
    explicit ThreadEndEvent(ShadowThreadMap::const_iterator shadow_thread,
                            std::unique_ptr<const ThreadEndInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {};

    /// Returns the event type.
    EventType type() const noexcept final;
};

/// @brief Output stream operator for ThreadEndEvent.
/// @param s The stream.
/// @param e The event to stream.
std::ostream& operator<<(std::ostream& s, const ThreadEndEvent& e);

#endif
