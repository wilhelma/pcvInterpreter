/**
 *
 *    @file  ThreadEndEvent.h
 *   @brief  
 *
 *    @date  06/29/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  THREAD_END_EVENT_H_
#define  THREAD_END_EVENT_H_

#include "fwd/ShadowThread.h"
#include "fwd/ThreadEndInfo.h"

#include "Event.h"

class ThreadEndEvent : public Event {
public:
	ThreadEndEvent(const ShadowThread *thread,
			       const ThreadEndInfo *info)
		: Event(thread), _info(info) {};
	virtual Events getEventType() const override final { return Events::THREADEND; };
	const ThreadEndInfo* const getThreadEndInfo() const { return _info; };

private:
	const ThreadEndInfo* const _info;

	// prevent generated functions
	ThreadEndEvent(const ThreadEndEvent&);
	ThreadEndEvent& operator=(const ThreadEndEvent&);
};

#endif
