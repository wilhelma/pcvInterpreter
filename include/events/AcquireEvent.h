/**
 *
 *    @file  AcquireEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACQUIRE_EVENT_H_
#define  ACQUIRE_EVENT_H_

#include "fwd/ShadowThread.h"
#include "AcquireInfo.h"

#include "Event.h"

class AcquireEvent : public Event {
public:
	AcquireEvent(const ShadowThread *thread,
				 const AcquireInfo *info) :
		Event(thread), _info(info) {};

	virtual Events getEventType() const override final { return Events::ACQUIRE; };
	const AcquireInfo* const getAcquireInfo() const { return _info; };

private:
	const AcquireInfo* const _info;

	// prevent generated functions
	AcquireEvent(const AcquireEvent&);
	AcquireEvent& operator=(const AcquireEvent&);
};

#endif
