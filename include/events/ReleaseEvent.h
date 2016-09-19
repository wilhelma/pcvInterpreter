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

#include "ReleaseInfo.h"
#include "fwd/ShadowLock.h"

#include "Event.h"

/// @ingroup events
/// @brief Event for a resource lock release.
class ReleaseEvent final : public Event<ReleaseInfo> {
public:
    /// @brief Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The release event information.
    explicit ReleaseEvent(const ShadowThread* thread, const ReleaseInfo* info) :
        Event(thread, info) {};

    /// Returns the event type.
    virtual Events getEventType() const override
    { return Events::RELEASE; }; 
};

#endif
