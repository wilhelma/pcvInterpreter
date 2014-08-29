/*
 * SAAPRunner.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "SAAPRunner.h"

bool SAAPRunner::registerTool(Tool* tool) {

	return true;
}

bool SAAPRunner::removeTool(Tool* tool) {

	return true;
}

void SAAPRunner::interpret() {

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
