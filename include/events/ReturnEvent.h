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

/// Event for function returns.
class ReturnEvent final : public Event<ReturnInfo> {
public:
	/// Constructor.
    /// @param  thread The thread the event was triggered from.
    /// @tparam info   The return event information.
	ReturnEvent(const ShadowThread* thread, const ReturnInfo* info) :
		Event(thread, info) {};

    /// Returns the event type.
    virtual Events getEventType() const override
    { return Events::RETURN; }
};

#endif
