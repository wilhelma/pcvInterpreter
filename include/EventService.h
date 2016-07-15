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
	EventService() {}
	bool publish(const AccessEvent *event) const;
	bool publish(const AcquireEvent *event) const;
	bool publish(const CallEvent *event) const;
	bool publish(const JoinEvent *event) const;
	bool publish(const NewThreadEvent *event) const;
	bool publish(const ReleaseEvent *event) const;
	bool publish(const ReturnEvent *event) const;
	bool publish(const ThreadEndEvent *event) const;
	bool subscribe(Tool* tool, const Filter* const filter, Events events);
	bool unsubscribe(Tool* tool);

private:
	// structures
	struct _observer {
		const Filter* const filter;
		Events events;

		explicit _observer(const Filter* const f, Events e) noexcept
			: filter(f), events(e)
		{}
	};

	// types
	typedef std::map<Tool*, const _observer> _observers_t;

	// private members
	_observers_t _observers;

	// prevent generated functions
	EventService(const EventService&);
	EventService& operator=(const EventService&);
};


#endif /* EVENTSERVICE_H_ */
