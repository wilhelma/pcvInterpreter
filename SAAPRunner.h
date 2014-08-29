/*
 * SAAPRunner.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef SAAPRUNNER_H_
#define SAAPRUNNER_H_

#include "Interpreter.h"
#include "EventService.h"

class SAAPRunner {
public:
	SAAPRunner(Interpreter* interpreter, EventService* eventservice) :
		_interpreter(interpreter), _eventService(eventservice) {}
	~SAAPRunner() {}

	bool registerTool(Tool* tool);
	bool removeTool(Tool* tool);
	void interpret();

private:
	// private data members
	Interpreter* _interpreter;
	EventService* _eventService;

	// prevent generated functions
	SAAPRunner(const SAAPRunner&);
	SAAPRunner& operator=(const SAAPRunner&);
};


#endif /* SAAPRUNNER_H_ */
