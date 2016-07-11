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

bool EventService::publish(const NewThreadEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::NEWTHREAD) != 0) 
			it.first->create(event);

	return true;
}

bool EventService::publish(const ThreadEndEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::THREADEND) != 0)
			it.first->create(event);

	return true;
}

bool EventService::publish(const JoinEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::JOIN) != 0)
			it.first->create(event);

	return true;
}

bool EventService::publish(const AcquireEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::ACQUIRE) != 0)
			it.first->acquire(event);

	return true;
}

bool EventService::publish(const ReleaseEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::RELEASE) != 0)
			it.first->release(event);

	return true;
}

bool EventService::publish(const ReturnEvent *event) const {
	for (const auto& it : _observers)
    if ((it.second.events & Events::RETURN) != 0)
			it.first->release(event);

	return true;
}

bool EventService::publish(const AccessEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::ACCESS) != 0)
			it.first->access(event);

	return true;
}

bool EventService::publish(const CallEvent *event) const {
	for (const auto& it : _observers)
		if ((it.second.events & Events::CALL) != 0)
			it.first->call(event);

	return true;
}

bool EventService::subscribe(Tool* tool, const Filter* const filter, Events events) {
	return _observers.insert(_observers_t::value_type(tool, _observer(filter, events))).second;
}

bool EventService::unsubscribe(Tool* tool) {
	return (_observers.erase(tool) > 0);
}
