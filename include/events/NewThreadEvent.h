/**
 *
 *    @file  NewThreadEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  NEW_THREAD_EVENT_H_
#define  NEW_THREAD_EVENT_H_

#include "fwd/NewThreadInfo.h"
#include "fwd/ShadowThread.h"

#include "Event.h"

/// Event for a new thread creation.
class NewThreadEvent final : public Event<NewThreadInfo> {
public:
	/// Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The new thread event information.
	NewThreadEvent(const ShadowThread* thread, const NewThreadInfo* info) :
		Event(thread, info) {};

    /// Returns the event type.
	virtual Events getEventType() const override
    { return Events::NEWTHREAD; };
};

#endif
