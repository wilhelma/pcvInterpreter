/**
 *
 *    @file  CallEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_EVENT_H_
#define  CALL_EVENT_H_

#include "fwd/CallInfo.h"
#include "fwd/ShadowThreadMap.h"

#include "Event.h"
#include "Types.h"

#include <iosfwd>
#include <memory>

/// @ingroup events
/// @brief Event for function calls.
class CallEvent final : public Event<CallInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The call event information.
    explicit CallEvent(ShadowThreadMap::const_iterator shadow_thread,
                       std::unique_ptr<const CallInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {};

    /// Return the event type.
    Events type() const final
    { return Events::CALL; };
};

/// @brief Output stream operator for CallEvent.
/// @param s The stream.
/// @param e The event to stream.
std::ostream& operator<<(std::ostream& s, const CallEvent& e);

#endif
