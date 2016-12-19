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
#include "fwd/ShadowThreadMap.h"

#include "Event.h"
#include "Types.h"

#include <iosfwd>
#include <memory>

/// @ingroup events
/// @brief Event for joining threads.
class JoinEvent final : public Event<JoinInfo> {
public:
    /// @brief  Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The join event information.
    explicit JoinEvent(ShadowThreadMap::const_iterator shadow_thread,
                       std::unique_ptr<const JoinInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {}

    /// Returns the event type.
    Events type() const final
    { return Events::JOIN; };
};

/// @brief Output stream operator for JoinEvent.
/// @param s The stream.
/// @param s The event to stream.
std::ostream& operator<<(std::ostream& s, const JoinEvent& e);

#endif
