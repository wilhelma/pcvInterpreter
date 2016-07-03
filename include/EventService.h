/*
 * EventService.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef EVENTSERVICE_H_
#define EVENTSERVICE_H_

#include <map>


#include "Event.h"
#include "AccessEvent.h"
#include "AcquireEvent.h"
#include "CallEvent.h"
#include "JoinEvent.h"
#include "NewThreadEvent.h"
#include "ReleaseEvent.h"
#include "ReturnEvent.h"
#include "ThreadEndEvent.h"

#include "Tool.h"
#include "Filter.h"

/******************************************************************************
 * EventService (Observable)
 *****************************************************************************/
class EventService {
public:
	EventService() {}
	bool publish(const AccessEvent *event);
	bool publish(const AcquireEvent *event);
	bool publish(const CallEvent *event);
	bool publish(const JoinEvent *event);
	bool publish(const NewThreadEvent *event);
	bool publish(const ReleaseEvent *event);
	bool publish(const ReturnEvent *event);
	bool publish(const ThreadEndEvent *event);
	bool subscribe(Tool* tool, const Filter* filter, enum Events events);
	bool unsubscribe(Tool* tool);

private:
	// structures
	struct _observers {
		const Filter* filter;
		Events events;
	};

	// types
	typedef std::map<Tool*, struct _observers> _observers_t;

	// private members
	_observers_t _observers;

	// prevent generated functions
	EventService(const EventService&);
	EventService& operator=(const EventService&);
};


#endif /* EVENTSERVICE_H_ */
