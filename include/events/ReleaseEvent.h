/**
 *
 *    @file  ReleaseEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RELEASE_EVENT_H_
#define  RELEASE_EVENT_H_

#include "fwd/ReleaseInfo.h"
#include "fwd/ShadowLock.h"

#include "Event.h"

class ReleaseEvent : public Event {
public:
	ReleaseEvent(const ShadowThread *thread,
			     const ReleaseInfo *info) :
		Event(thread), _info(info) {};

	virtual Events getEventType() const override final { return Events::RELEASE; }; 
	const ReleaseInfo* const getReleaseInfo() const { return _info; };

private:
	const ReleaseInfo* const _info;

	// prevent generated functions
	ReleaseEvent(const ReleaseEvent&);
	ReleaseEvent& operator=(const ReleaseEvent&);
};

#endif
