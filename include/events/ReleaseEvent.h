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

#ifndef  RELEASE_INFO_H_
#define  RELEASE_INFO_H_

#include "Event.h"
#include "ShadowLock.h"

struct ReleaseInfo {
	ShadowLock *lock;
	ReleaseInfo(ShadowLock* lock) : lock(lock) {}
};

class ReleaseEvent : public Event {
public:
	ReleaseEvent(const ShadowThread *thread,
				 const struct ReleaseInfo *info) :
					 Event(thread), _info(info) {}

	Events getEventType() const override { return Events::RELEASE; }
	const ReleaseInfo* getReleaseInfo() const { return _info; };

private:
	const struct ReleaseInfo *_info;

	// prevent generated functions
	ReleaseEvent(const ReleaseEvent&);
	ReleaseEvent& operator=(const ReleaseEvent&);
};

#endif
