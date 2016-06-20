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

#ifndef  ACCESS_INFO_H_
#define  ACCESS_INFO_H_

#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "Types.h"

struct AccessInfo {
	AccessInfo( AccessType Type, ShadowVar *Var, INS_ID instructionID)
		: type(Type), instructionID(instructionID), var(Var) {}

	AccessType type;
	INS_ID instructionID;
	ShadowVar *var;
};

class AccessEvent : public Event {
public:
	AccessEvent(const ShadowThread *thread,
				const struct AccessInfo *info) :
					 Event(thread), _info(info) {}

	Events getEventType() const override { return Events::ACCESS; }
	const AccessInfo *getAccessInfo() const { return _info; };

private:
	const struct AccessInfo *_info;

	// prevent generated functions
	AccessEvent(const AccessEvent&);
	AccessEvent& operator=(const AccessEvent&);
};


#endif
