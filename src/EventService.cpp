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

bool EventService::publish(const NewThreadEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it)
		if ((it->second.events & Events::NEWTHREAD) != 0) 
			it->first->create(event);

	return true;
}

bool EventService::publish(const ThreadEndEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
    if ( (it->second.events | Events::THREADEND) != 0 ) {
			it->first->create(event);
		}
	}

	return true;
}

bool EventService::publish(const JoinEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it)
		if ((it->second.events & Events::JOIN) != 0)
			it->first->create(event);

	return true;
}

bool EventService::publish(const AcquireEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it)
		if ((it->second.events & Events::ACQUIRE) != 0)
			it->first->acquire(event);

	return true;
}

bool EventService::publish(const ReleaseEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it)
		if ((it->second.events & Events::RELEASE) != 0)
			it->first->release(event);

	return true;
}

bool EventService::publish(const ReturnEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
    if ( (it->second.events | Events::RETURN) != 0 ) {
			it->first->release(event);
		}
	}

	return true;
}

bool EventService::publish(const AccessEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it)
		if ((it->second.events & Events::ACCESS) != 0)
			it->first->access(event);

	return true;
}

bool EventService::publish(const CallEvent *event) {
	_observers_t::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it)
		if ((it->second.events & Events::CALL) != 0)
			it->first->call(event);

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
