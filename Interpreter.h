/*
 * Interpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

class EventService;

/******************************************************************************
 * Interpreter (abstract)
 *****************************************************************************/
class Interpreter {
public:
	virtual int process() = 0;
	virtual ~Interpreter() {};

private:
	EventService *_eventService;
};


#endif /* INTERPRETER_H_ */
