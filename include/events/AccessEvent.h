/**
 *
 *    @file  AccessEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_EVENT_H_
#define  ACCESS_EVENT_H_

#include "fwd/AccessInfo.h"
#include "fwd/ShadowThread.h"

#include "Event.h"
#include "Access.h"
#include "Types.h"


class AccessEvent : public Event {
public:
	AccessEvent(const ShadowThread *thread,
				const AccessInfo *info) :
		Event(thread), _info(info) {};

	virtual Events getEventType() const override final { return Events::ACCESS; };
	const AccessInfo* const getAccessInfo() const { return _info; };

private:
	const AccessInfo* const _info;

	// prevent generated functions
	AccessEvent(const AccessEvent&);
	AccessEvent& operator=(const AccessEvent&);
};


#endif
