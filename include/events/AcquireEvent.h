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

#include "Event.h"
#include "ShadowLock.h"

struct AcquireInfo {
	ShadowLock *lock;
	AcquireInfo(ShadowLock *lock) : lock(lock) {}
};

class AcquireEvent : public Event {
public:
	AcquireEvent(const ShadowThread *thread,
				 const struct AcquireInfo *info) :
					 Event(thread), _info(info) {}
	Events getEventType() const override { return Events::ACQUIRE; }
	const AcquireInfo* getAcquireInfo() const { return _info; };

private:
	const struct AcquireInfo *_info;

	// prevent generated functions
	AcquireEvent(const AcquireEvent&);
	AcquireEvent& operator=(const AcquireEvent&);
};

#endif
