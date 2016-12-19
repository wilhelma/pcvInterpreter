/*
 * EventService.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "EventService.h"

#include "EventType.h"
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
        switch (it.events & EventType::NEWTHREAD) {
            case EventType::NEWTHREAD:
                it.tool->NewThread(event);
                break;
            default:
                break;
        } 

    return true;
}

bool EventService::publish(const ThreadEndEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::THREADEND) {
            case EventType::THREADEND:
                it.tool->ThreadEnd(event);
                break;
            default:
                break;
        }

    return true;
}

bool EventService::publish(const JoinEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::JOIN) {
            case EventType::JOIN:
                it.tool->Join(event);
                break;
            default:
                break;
        }

    return true;
}

bool EventService::publish(const AcquireEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::ACQUIRE) {
            case EventType::ACQUIRE:
                it.tool->Acquire(event);
                break;
            default:
                break;
        }

    return true;
}

bool EventService::publish(const ReleaseEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::RELEASE) {
            case EventType::RELEASE:
                it.tool->Release(event);
                break;
            default:
                break;
        }

    return true;
}

bool EventService::publish(const ReturnEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::RETURN) {
            case EventType::RETURN:
                it.tool->Return(event);
                break;
            default:
                break;
        }

    return true;
}

bool EventService::publish(const AccessEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::ACCESS) {
            case EventType::ACCESS:
                it.tool->Access(event);
                break;
            default:
                break;
        }

    return true;
}

bool EventService::publish(const CallEvent *event) const {
    for (const auto& it : *observerList())
        switch (it.events & EventType::CALL) {
            case EventType::CALL:
                it.tool->Call(event);
                break;
            default:
                break;
        }

    return true;
}
