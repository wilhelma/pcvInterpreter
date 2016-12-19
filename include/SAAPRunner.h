/*
 * SAAPRunner.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef SAAPRUNNER_H_
#define SAAPRUNNER_H_

#include "fwd/DBInterpreter.h"
#include "fwd/EventType.h"
#include "fwd/Filter.h"
#include "fwd/Observer.h"
#include "fwd/Tool.h"

#include <string>
#include <memory>

/// Controls the interpretation of the database and manages the list of observers.
class SAAPRunner {
public:
	/// Constructor.
	explicit SAAPRunner();
	/// _Default_ destructor.
	~SAAPRunner();

	/// _Deleted_ copy constructor.
	SAAPRunner(const SAAPRunner&)            = delete;
	/// _Deleted_ move constructor.
	SAAPRunner(SAAPRunner&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	SAAPRunner& operator=(const SAAPRunner&) = delete;
	/// _Deleted_ move assignment operator.
	SAAPRunner& operator=(SAAPRunner&&)      = delete;

	/// @brief Subscribes the tool to the EventService.
	/// @param tool   Pointer to the Tool to register.
	/// @param filter Pointer to the Filter.
	/// @param events Events the tool subscribes to.
    /// @return An iterator to the element that has been subscribed to the EventService.
    /// @attention The returned iterator will be used to unsubscribe the tool (via
    /// `SAAPRunner::removeTool`). __That's the only chance to get it, so make sure
    /// to store it!__
    const ObserverList::const_iterator registerTool(std::unique_ptr<Tool>&& tool,
                                                    std::unique_ptr<Filter>&& filter,
                                                    EventType&& events) const;

    /// @brief Unsubscribes the Tool from the events and deletes it.
    /// @param tool Iterator to the Tool to remove.
    /// @attention The Tool iterator is given when registering
    /// the Tool to the SAAPRunner.
    void removeTool(ObserverList::const_iterator tool) const noexcept;

	/// @brief Interprets the database.
	/// @param DBPath The database file to interpret.
	void interpret(const std::string& DBPath) const;

private:
    /// _Non-constant_ pointer to the observer list.
    std::shared_ptr<ObserverList> ObserverList_;

	/// @brief Pointer to the database interpreter.
    /// @todo Try to make this constant!
	std::unique_ptr<DBInterpreter> DBInterpreter_;
};

#endif
