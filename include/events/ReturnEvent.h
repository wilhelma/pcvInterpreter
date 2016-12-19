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

#include "fwd/EventType.h"
#include "fwd/ReturnInfo.h"
#include "fwd/ShadowThreadMap.h"

#include "Event.h"
#include "Types.h"

#include <iosfwd>
#include <memory>

/// @ingroup events
/// @brief Event for function returns.
class ReturnEvent final : public Event<ReturnInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The return event information.
    explicit ReturnEvent(ShadowThreadMap::const_iterator shadow_thread,
                         std::unique_ptr<const ReturnInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {};

    /// Returns the event type.
    EventType type() const noexcept final;
};

/// @brief Output stream operator for ReturnEvent.
/// @param s The stream.
/// @param e The event to stream.
std::ostream& operator<<(std::ostream& s, const ReturnEvent& e);

#endif
