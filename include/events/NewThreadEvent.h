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

class NewThreadEvent : public Event {
public:
	NewThreadEvent(const ShadowThread *thread,
				   const NewThreadInfo *info) :
		Event(thread), _info(info) {};

	virtual Events getEventType() const override final { return Events::NEWTHREAD; };
	const NewThreadInfo* const getNewThreadInfo() const { return _info; };

private:
	const NewThreadInfo* const _info;

	// prevent generated functions
    NewThreadEvent(const NewThreadEvent&);
	NewThreadEvent& operator=(const NewThreadEvent&);
};

#endif
