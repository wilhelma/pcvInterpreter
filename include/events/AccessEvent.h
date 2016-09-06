/**
 *
 *    @file  AccessEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_EVENT_H_
#define  ACCESS_EVENT_H_

#include "fwd/AccessInfo.h"
#include "fwd/ShadowThread.h"

#include "Event.h"

/// Event for memory access.
class AccessEvent final : public Event<AccessInfo> {
public:
	/// Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The access event information.
	AccessEvent(const ShadowThread* thread, const AccessInfo* info) :
		Event(thread, info) {};

    /// Return the event type.
	virtual Events getEventType() const override
    { return Events::ACCESS; };
};


#endif
