/*
 * EventService.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef EVENTSERVICE_H_
#define EVENTSERVICE_H_

#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/CallEvent.h"
#include "fwd/Filter.h"
#include "fwd/JoinEvent.h"
#include "fwd/NewThreadEvent.h"
#include "fwd/ReleaseEvent.h"
#include "fwd/ReturnEvent.h"
#include "fwd/ThreadEndEvent.h"
#include "fwd/Tool.h"

#include "Event.h"

#include <map>

/// \class EventService (observable)
class EventService {
public:
	/// (Default) constructor.
	EventService() {};

	/// @brief Puiblish an `AccessEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const AccessEvent *event) const;

	/// @brief Puiblish an `AcquireEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const AcquireEvent *event) const;

	/// @brief Puiblish an `CallEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const CallEvent *event) const;

	/// @brief Puiblish an `JoinEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const JoinEvent *event) const;

	/// @brief Puiblish an `NewThreadEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const NewThreadEvent *event) const;

	/// @brief Puiblish an `ReleaseEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const ReleaseEvent *event) const;

	/// @brief Puiblish an `ReturnEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const ReturnEvent *event) const;

	/// @brief Puiblish an `ThreadEndEvent` to the registered tools.
	/// @param event The event to publish.
	bool publish(const ThreadEndEvent *event) const;

	/// @brief Subscribes a tool to the `EventService`.
	bool subscribe(Tool* const tool, const Filter* const filter, Events events) {
		return Observers_.insert( std::map<Tool* const, const Observer>::value_type(
					tool, Observer(filter, events))).second; }

	/// @brief Unsubscribes a tool to the `EventService`.
	bool unsubscribe(Tool* const tool)
	{ return (Observers_.erase(tool) > 0); }
		

private:
	// structures
	struct Observer {
		const Filter* const filter;
		Events events;

		explicit Observer(const Filter* const f, Events e) noexcept
			: filter(f), events(e)
		{}
	};

	/// Maps every registered tool to its filter and the events
	/// it subscribed for.
	std::map<Tool* const, const Observer> Observers_;

	// prevent generated functions
	EventService(const EventService&);
	EventService& operator=(const EventService&);
};


#endif /* EVENTSERVICE_H_ */
