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

#include "Event.h"
#include "Types.h"

typedef struct ReturnInfo {
	CAL_ID call;
	TIME   endTime;

	ReturnInfo(CAL_ID call,
			   TIME endTime)
		: call(call), endTime(endTime)
	{}
} ReturnInfo;

class ReturnEvent : public Event {
	public:
<<<<<<< 90d304cc9d200cb126445e5dbe4ddb6f6e7f3ba9
		ReturnEvent(const ShadowThread *thread,
				    const ReturnInfo *info) :
			Event(thread), _info(info) {}

		virtual Events getEventType() const override final { return Events::RETURN; }
		const ReturnInfo* const getReturnInfo() const { return _info; };
	private:
		const ReturnInfo* const _info;
=======
		Events getEventType() const override { return Events::RETURN; }
>>>>>>> create separate files for different events
};

#endif
