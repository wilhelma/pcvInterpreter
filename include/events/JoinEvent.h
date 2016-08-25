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

/// Event for joining threads.
class JoinEvent final : public Event<JoinInfo> {
public:
	/// Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The join event information.
	JoinEvent(const ShadowThread* thread, const JoinInfo* info) :
		Event(thread, info) {};

    /// Returns the event type.
	virtual Events getEventType() const override
    { return Events::JOIN; };
};

#endif
