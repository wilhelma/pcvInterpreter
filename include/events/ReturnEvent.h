/**
 *
 *    @file  ReturnEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RETURN_EVENT_H_
#define  RETURN_EVENT_H_

#include "fwd/ReturnInfo.h"
#include "fwd/ShadowThread.h"

#include "Event.h"

#include <memory>

/// @ingroup events
/// @brief Event for function returns.
class ReturnEvent final : public Event<ReturnInfo> {
public:
    /// @brief Constructor.
    /// @param  thread The thread the event was triggered from.
    /// @tparam info   The return event information.
    explicit ReturnEvent(std::shared_ptr<const ShadowThread> thread,
                         std::unique_ptr<const ReturnInfo>&& info)
        : Event(thread, std::move(info))
    {};

    /// Returns the event type.
    Events type() const final
    { return Events::RETURN; }
};

#endif
