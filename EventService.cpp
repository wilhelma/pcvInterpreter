/*
 * EventService.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include <iostream>
#include "EventService.h"
#include "Filter.h"
#include "Tool.h"
#include "Event.h"

bool EventService::publish(NewThreadEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it->second.events && NEWTHREAD ) {
			it->first->create(event);
		}
	}

	return true;
}

bool EventService::publish(JoinEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it->second.events && NEWTHREAD ) {
			it->first->create(event);
		}
	}

	return true;
}

bool EventService::publish(AcquireEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it->second.events && ACQUIRE ) {
			it->first->acquire(event);
		}
	}

	return true;
}

bool EventService::publish(ReleaseEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it->second.events && RELEASE ) {
			it->first->release(event);
		}
	}

	return true;
}

bool EventService::publish(AccessEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it->second.events && ACCESS ) {
			it->first->access(event);
		}
	}

	return true;
}

bool EventService::publish(CallEvent *event)
{
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it->second.events && CALL ) {
			it->first->call(event);
		}
	}

	return true;
}

bool EventService::subscribe(Tool* tool,
							 const Filter* filter,
							 enum Events events) {

	if (_observers.find(tool) != _observers.end())
		return false;

	struct _observers obj;
	obj.filter = filter;
	obj.events = events;
	_observers[tool] = obj;

	return true;
}

bool EventService::unsubscribe(Tool* tool) {

	return (_observers.erase(tool) > 0);
}
