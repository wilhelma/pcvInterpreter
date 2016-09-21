/*
 * EventService.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef EVENT_SERVICE_H_
#define EVENT_SERVICE_H_

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

#include "fwd/Filter.h"
#include "fwd/SAAPRunner.h"
#include "fwd/Tool.h"

#include "Event.h"
#include "Observer.h"

#include <list>
#include <memory>

/// @brief The observable: keeps track of the register observed and
/// `publish()`es them the Event's received from the DBInterpreter.
class EventService {
public:
    /// _Default_ constructor.
    explicit EventService() = default;
    /// _Default_ destructor.
    ~EventService()                   = default;

    /// _Deleted_ copy constructor.
    EventService(const EventService&)            = delete;
    /// _Deleted_ move constructor.
    EventService(EventService&&)                 = delete;
    /// _Deleted_ copy assignment operator.
    EventService& operator=(const EventService&) = delete;
    /// _Deleted_ move assignment operator.
    EventService& operator=(EventService&)       = delete;

    /// @brief Puiblish an `AccessEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const AccessEvent *event) const;

    /// @brief Puiblish an `AcquireEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const AcquireEvent *event) const;

    /// @brief Puiblish a `CallEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const CallEvent *event) const;

    /// @brief Puiblish a `JoinEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const JoinEvent *event) const;

    /// @brief Puiblish a `NewThreadEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const NewThreadEvent *event) const;

    /// @brief Puiblish a `ReleaseEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const ReleaseEvent *event) const;

    /// @brief Puiblish a `ReturnEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const ReturnEvent *event) const;

    /// @brief Puiblish a `ThreadEndEvent` to the registered tools.
    /// @param event The event to publish.
    bool publish(const ThreadEndEvent *event) const;

    /// @brief Grant SAAPRunner the permission to access subscribe and unsubscribe.
    /// @todo Give SAAPRunner only access to subscribe and unsubscribe
    /// @todo Give DBInterpreter only access to publish functions
    friend SAAPRunner;

private:

    /// @brief Subscribes a tool to the observable.
    /// @param tool   The Tool to subscribe.
    /// @param filter The Filter for the events.
    /// @param events The Events the Tool is registered to.
    /// @return A reference to the inserted element.
    decltype(auto) subscribe(std::unique_ptr<Tool>&& tool,
                             std::unique_ptr<Filter>&& filter,
                             Events&& events)
    { return ObserverList_.emplace(ObserverList_.cend(), Observer(std::move(tool), std::move(filter), std::move(events))); }

    /// @brief Unsubscribes a tool to the `EventService`.
    /// @param tool Iterator to the tool to remove.
    /// @attention The Tool iterator is given when registering
    /// the Tool to the SAAPRunner.
    void unsubscribe(std::list<Observer>::const_iterator tool)
    { ObserverList_.erase(tool); }

    /// List of registered observers.
    std::list<Observer> ObserverList_;
};


#endif /* EVENTSERVICE_H_ */
