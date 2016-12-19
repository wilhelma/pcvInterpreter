/**
 *
 *    @file  ReleaseEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RELEASE_EVENT_H_
#define  RELEASE_EVENT_H_

#include "fwd/ReleaseInfo.h"
#include "fwd/ShadowThreadMap.h"

#include "Event.h"
#include "Types.h"

#include <iosfwd>
#include <memory>

/// @ingroup events
/// @brief Event for a resource lock release.
class ReleaseEvent final : public Event<ReleaseInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The release event information.
    explicit ReleaseEvent(ShadowThreadMap::const_iterator shadow_thread,
                          std::unique_ptr<const ReleaseInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {}; 

    /// Returns the event type.
    Events type() const noexcept final
    { return Events::RELEASE; }; 
};

/// @brief Output stream operator for ReleaseEvent.
/// @param s The stream.
/// @param e The event to stram.
std::ostream& operator<<(std::ostream& s, const ReleaseEvent& e);

#endif
