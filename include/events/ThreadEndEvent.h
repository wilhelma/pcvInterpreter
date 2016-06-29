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

#ifndef  thread_end_event_H_
#define  thread_end_event_H_

#include "Event.h"
#include "Types.h"

struct ThreadEndInfo {
};

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
