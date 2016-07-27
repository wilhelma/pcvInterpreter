/*
 * SAAPRunner.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef SAAPRUNNER_H_
#define SAAPRUNNER_H_

#include "fwd/Filter.h"
#include "fwd/Tool.h"

#include "Event.h"
#include "EventService.h"
#include "Interpreter.h"


#include <string>
#include <memory>

class SAAPRunner {
public:
	/// Constructor.
	SAAPRunner(Interpreter *interpreter) :
		Interpreter_(interpreter)
	{}

	/// @brief Subscribes the tool to the event service.
	/// @param tool   Ptr to the tool to register.
	/// @param filter Ptr to the filter.
	/// @param events Events the tool subscribes to.
	bool registerTool(Tool* tool, const Filter* filter, Events events)
	{ return Interpreter_->getEventService()->subscribe(tool, filter, events); }

	/// @brief Unsubscribes the tool from the event service.
	/// @param tool Ptr to the tool to unregister.
	bool removeTool(Tool* tool)
	{ return Interpreter_->getEventService()->unsubscribe(tool); }

	/// @brief Interprets the database.
	/// @details Calls `Interpreter::process`.
	/// @param DBPath The database file to interpret.
	void interpret(const std::string& DBPath)
	{ Interpreter_->process(DBPath); };

private:
//	/// Private unique pointer to the `Interpreter`.
//	std::unique_ptr<Interpreter> Interpreter_;

	Interpreter* Interpreter_;

	// prevent generated functions
	SAAPRunner(const SAAPRunner&);
	SAAPRunner& operator=(const SAAPRunner&);
};

#endif /* SAAPRUNNER_H_ */
