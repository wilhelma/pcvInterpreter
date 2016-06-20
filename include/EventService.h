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
#include "Tool.h"
#include "Filter.h"

/******************************************************************************
 * EventService (Observable)
 *****************************************************************************/
class EventService {
public:
	EventService() {}
	bool publish(NewThreadEvent *event);
	bool publish(JoinEvent *event);
	bool publish(AcquireEvent *event);
	bool publish(ReleaseEvent *event);
	bool publish(ReturnEvent *event);
	bool publish(AccessEvent *event);
	bool publish(CallEvent *event);
	bool subscribe(Tool* tool, const Filter* filter, enum Events events);
	bool unsubscribe(Tool* tool);

private:
	// structures
	struct _observers {
		const Filter* filter;
		enum Events events;
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
