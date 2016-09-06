/*
 * EventService.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "EventService.h"
#include "Tool.h"
#include "Event.h"

bool EventService::publish(const NewThreadEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::NEWTHREAD) != 0) 
            it.tool->NewThread(event);

    return true;
}

bool EventService::publish(const ThreadEndEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::THREADEND) != 0)
            it.tool->ThreadEnd(event);

    return true;
}

bool EventService::publish(const JoinEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::JOIN) != 0)
            it.tool->Join(event);

    return true;
}

bool EventService::publish(const AcquireEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::ACQUIRE) != 0)
            it.tool->Acquire(event);

    return true;
}

bool EventService::publish(const ReleaseEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::RELEASE) != 0)
            it.tool->Release(event);

    return true;
}

bool EventService::publish(const ReturnEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::RETURN) != 0)
            it.tool->Return(event);

    return true;
}

bool EventService::publish(const AccessEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::ACCESS) != 0)
            it.tool->Access(event);

    return true;
}

bool EventService::publish(const CallEvent *event) const {
    for (const auto& it : ObserverList_)
        if ((it.events & Events::CALL) != 0)
            it.tool->Call(event);

    return true;
}
