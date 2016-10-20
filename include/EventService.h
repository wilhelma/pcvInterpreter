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

#include "fwd/Observer.h"

#include <memory>

/// @brief The observable: keeps track of the register observed and
/// `publish()`es them the Event's received from the DBInterpreter.
class EventService {
public:
    /// @brief Constructor.
    /// @param observer_list The observer list provided by the SAAPRunner.
    explicit EventService(const std::shared_ptr<const ObserverList>& observer_list);
    /// _Default_ destructor.
    ~EventService() = default;

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

    /// Return the list of observers.
    const std::shared_ptr<const ObserverList>& observerList() const noexcept
    { return ObserverList_; }

private:
    /// List of registered observers.
    const std::shared_ptr<const ObserverList> ObserverList_;
};

#endif
