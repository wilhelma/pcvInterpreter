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

class ReturnEvent : public Event {
	public:
		ReturnEvent(const ShadowThread *thread,
				    const ReturnInfo *info) :
			Event(thread), _info(info) {}

		virtual Events getEventType() const override final { return Events::RETURN; }
		const ReturnInfo* const getReturnInfo() const { return _info; };
	private:
		const ReturnInfo* const _info;
};

#endif
