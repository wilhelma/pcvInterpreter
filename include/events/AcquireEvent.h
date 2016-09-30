/**
 *
 *    @file  AcquireEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACQUIRE_EVENT_H_
#define  ACQUIRE_EVENT_H_

#include "fwd/ShadowThread.h"
#include "fwd/AcquireInfo.h"

#include "Event.h"

/// @ingroup events
/// @todo What is this for?
class AcquireEvent final : public Event<AcquireInfo> {
public:
    /// @brief Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The acquire event information.
    explicit AcquireEvent(const ShadowThread* thread, const AcquireInfo* info) :
        Event(thread, info) {};

    /// Return the event type.
    virtual Events getEventType() const override
    { return Events::ACQUIRE; };
};

#endif
