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
#include "fwd/ShadowThread.h"

#include "Event.h"

/// Event for function calls.
class CallEvent final : public Event<CallInfo> {
public:
	/// Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The call event information.
	CallEvent(const ShadowThread* thread, const CallInfo* info) :
		Event(thread, info) {};

    /// Return the event type.
    virtual Events getEventType() const override
    { return Events::CALL; };
};

#endif
