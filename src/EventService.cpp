/*
 * EventService.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "EventService.h"

#include "Event.h"
#include "Observer.h"
#include "Tool.h"

// Events ---------------------
#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/CallEvent.h"
#include "fwd/JoinEvent.h"
#include "fwd/NewThreadEvent.h"
#include "fwd/ReleaseEvent.h"
#include "fwd/ReturnEvent.h"
#include "fwd/ThreadEndEvent.h"
// ----------------------------

#include "fwd/Observer.h"

EventService::EventService(const std::shared_ptr<const ObserverList>& observer_list) :
    ObserverList_(observer_list)
{}

bool EventService::publish(const NewThreadEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::NEWTHREAD) != 0) 
            it.tool->NewThread(event);

    return true;
}

bool EventService::publish(const ThreadEndEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::THREADEND) != 0)
            it.tool->ThreadEnd(event);

    return true;
}

bool EventService::publish(const JoinEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::JOIN) != 0)
            it.tool->Join(event);

    return true;
}

bool EventService::publish(const AcquireEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::ACQUIRE) != 0)
            it.tool->Acquire(event);

    return true;
}

bool EventService::publish(const ReleaseEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::RELEASE) != 0)
            it.tool->Release(event);

    return true;
}

bool EventService::publish(const ReturnEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::RETURN) != 0)
            it.tool->Return(event);

    return true;
}

bool EventService::publish(const AccessEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::ACCESS) != 0)
            it.tool->Access(event);

    return true;
}

bool EventService::publish(const CallEvent *event) const {
    for (const auto& it : *observerList())
        if ((it.events & Events::CALL) != 0)
            it.tool->Call(event);

    return true;
}
