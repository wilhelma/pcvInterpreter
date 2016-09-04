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

#include <list>
#include <string>
#include <memory>

class SAAPRunner {
public:
	/// Constructor.
    /// @param interpreter Pointer to the database interpreter.
	SAAPRunner(std::unique_ptr<DBInterpreter>&& interpreter) :
		DBInterpreter_(std::move(interpreter))
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

	/// @brief Subscribes the tool to the EventService.
	/// @param tool   Pointer to the Tool to register.
	/// @param filter Pointer to the Filter.
	/// @param events Events the tool subscribes to.
    /// @return An iterator to the element that has been
    /// subscribed to the EventService.
    /// @attention The returned iterator will be used to
    /// unsubscribe the tool (via `SAAPRunner::removeTool`).
    /// __That's the only chance to get it, so make sure to store it!__
    decltype(auto) registerTool(std::unique_ptr<Tool>&& tool,
                                std::unique_ptr<Filter>&& filter,
                                Events&& events)
    { return DBInterpreter_->getEventService()->subscribe(
            std::move(tool), std::move(filter), std::move(events)); }

    /// @brief Unsubscribes the Tool from the events and deletes it.
    /// @param tool Iterator to the Tool to remove.
    /// @attention The Tool iterator is given when registering
    /// the Tool to the SAAPRunner.
    void removeTool(std::list<Observer>::const_iterator tool)
	{ DBInterpreter_->getEventService()->unsubscribe(tool); }

	/// @brief Interprets the database.
	/// @details Calls `Interpreter::process`.
	/// @param DBPath The database file to interpret.
	void interpret(const std::string& DBPath)
	{ DBInterpreter_->process(DBPath); };

private:
	/// Pointer to the database interpreter.
	std::unique_ptr<DBInterpreter> DBInterpreter_;
};

#endif /* SAAPRUNNER_H_ */
