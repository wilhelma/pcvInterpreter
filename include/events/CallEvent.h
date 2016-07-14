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
#include "CallInfo.h"
#include "fwd/ShadowThread.h"
#include "Event.h"


class CallEvent : public Event {
	public:
		CallEvent(const ShadowThread *thread,
				  const CallInfo *info) :
			Event(thread), _info(info) {};

		virtual Events getEventType() const override final { return Events::CALL; };
		const CallInfo* const getCallInfo() const { return _info; };

	private:
		const CallInfo* const _info;

		// prevent generated functions
		CallEvent(const CallEvent&);
		CallEvent& operator=(const CallEvent&);
};

#endif
