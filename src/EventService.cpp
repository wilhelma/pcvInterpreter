/*
 * EventService.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "EventService.h"
#include "Tool.h"
#include "Event.h"

#include <iostream>

bool EventService::publish(const NewThreadEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::NEWTHREAD) != 0) 
			it.first->NewThread(event);

	return true;
}

bool EventService::publish(const ThreadEndEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::THREADEND) != 0)
			it.first->ThreadEnd(event);

	return true;
}

bool EventService::publish(const JoinEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::JOIN) != 0)
			it.first->Join(event);

	return true;
}

bool EventService::publish(const AcquireEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::ACQUIRE) != 0)
			it.first->Acquire(event);

	return true;
}

bool EventService::publish(const ReleaseEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::RELEASE) != 0)
			it.first->Release(event);

	return true;
}

bool EventService::publish(const ReturnEvent *event) const {
	for (const auto& it : Observers_)
    if ((it.second.events & Events::RETURN) != 0)
		it.first->Return(event);

	return true;
}

bool EventService::publish(const AccessEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::ACCESS) != 0)
			it.first->Access(event);

	return true;
}

bool EventService::publish(const CallEvent *event) const {
	for (const auto& it : Observers_)
		if ((it.second.events & Events::CALL) != 0)
			it.first->Call(event);

	return true;
}
