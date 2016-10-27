/**
 *
 *    @file  JoinEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  JOIN_EVENT_H_
#define  JOIN_EVENT_H_

#include "fwd/JoinInfo.h"
#include "fwd/ShadowThread.h"

#include "Event.h"

#include <memory>

/// @ingroup events
/// @brief Event for joining threads.
class JoinEvent final : public Event<JoinInfo> {
public:
    ///@brief  Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The join event information.
    explicit JoinEvent(std::shared_ptr<const ShadowThread> thread,
                       std::unique_ptr<const JoinInfo>&& info) :
        Event(thread, std::move(info)) {};

    /// Returns the event type.
    Events type() const final
    { return Events::JOIN; };
};

#endif
