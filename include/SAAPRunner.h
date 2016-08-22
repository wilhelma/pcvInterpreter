/*
 * SAAPRunner.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef SAAPRUNNER_H_
#define SAAPRUNNER_H_

#include "fwd/Event.h"
#include "fwd/Filter.h"
#include "fwd/Tool.h"

#include "EventService.h"
#include "DBInterpreter.h"

#include <string>
#include <memory>

class SAAPRunner {
public:
	/// Constructor.
	SAAPRunner(DBInterpreter *interpreter) :
		DBInterpreter_(interpreter)
	{}

	/// _Deleted_ copy constructor.
	SAAPRunner(const SAAPRunner&)            = delete;
	/// _Deleted_ move constructor.
	SAAPRunner(SAAPRunner&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	SAAPRunner& operator=(const SAAPRunner&) = delete;
	/// _Deleted_ move assignment operator.
	SAAPRunner& operator=(SAAPRunner&&)      = delete;
	/// _Default_ destructor.
	~SAAPRunner()                            = default;

	/// @brief Subscribes the tool to the event service.
	/// @param tool   Ptr to the tool to register.
	/// @param filter Ptr to the filter.
	/// @param events Events the tool subscribes to.
	bool registerTool(Tool* tool, const Filter* filter, Events events)
	{ return DBInterpreter_->getEventService()->subscribe(tool, filter, events); }

	/// @brief Unsubscribes the tool from the event service.
	/// @param tool Ptr to the tool to unregister.
	bool removeTool(Tool* tool)
	{ return DBInterpreter_->getEventService()->unsubscribe(tool); }

	/// @brief Interprets the database.
	/// @details Calls `Interpreter::process`.
	/// @param DBPath The database file to interpret.
	void interpret(const std::string& DBPath)
	{ DBInterpreter_->process(DBPath); };

private:
	/// Private unique pointer to the `Interpreter`.
	std::unique_ptr<DBInterpreter> DBInterpreter_;

//	DBInterpreter* DBInterpreter_;
};

#endif /* SAAPRUNNER_H_ */
