/*
 * SAAPRunner.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "SAAPRunner.h"
#include "EventService.h"
#include "Interpreter.h"
#include "Filter.h"
#include "Event.h"

bool SAAPRunner::registerTool(Tool* tool,
							  const Filter* filter,
							  enum Events events)
{
	_interpreter->getEventService()->subscribe(tool, filter, events);
	return true;
}

bool SAAPRunner::removeTool(Tool* tool)
{
	_interpreter->getEventService()->unsubscribe(tool);
	return true;
}
