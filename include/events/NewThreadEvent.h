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

#include "Event.h"
#include "ShadowThread.h"
#include "Types.h"

struct NewThreadInfo {
  ShadowThread* childThread;
  ShadowThread* parentThread;
  NUM_CYCLES runtime;
  TIME startTime;
  NewThreadInfo(ShadowThread* childThread,
                ShadowThread* parentThread,
                NUM_CYCLES runtime,
                TIME_STRING startTimeString)
    : childThread(childThread), parentThread(parentThread),
	  runtime(runtime), startTime(timeStringToTime(startTimeString)) {}
};

class NewThreadEvent : public Event {
public:
	NewThreadEvent(const ShadowThread *thread,
				   const NewThreadInfo* const info) :
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
