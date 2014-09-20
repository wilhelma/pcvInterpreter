/*
 * SAAPRunner.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include <iostream>

#include "SAAPRunner.h"
#include "EventService.h"
#include "Interpreter.h"
#include "Filter.h"
#include "Event.h"

bool SAAPRunner::registerTool(Tool* tool,
							  const Filter* filter,
							  enum Events events)
{
	EventService *service = _interpreter->getEventService();
	service->subscribe(tool, filter, events);
	return true;
}

bool SAAPRunner::removeTool(Tool* tool)
{
	EventService *service = _interpreter->getEventService();
	service->unsubscribe(tool);
	return true;
}

void SAAPRunner::interpret()
{
	_interpreter->process();
}

//class SAAPRunner {
//public:
//	SAAPRunner(Interpreter* interpreter, EventService* eventservice) :
//		_interpreter(interpreter), _eventService(eventservice) {}
//	~SAAPRunner() {}
//
//	bool registerTool(Tool* tool);
//	bool removeTool(Tool* tool);
//	void interpret();
//
//private:
//	// private data members
//	Interpreter* _interpreter;
//	EventService* _eventService;
//
//	// prevent generated functions
//	SAAPRunner(const SAAPRunner&);
//	SAAPRunner& operator=(const SAAPRunner&);
//};
