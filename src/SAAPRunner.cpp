/**
 *
 *    @file  SAAPRunner.cpp
 *   @brief  
 *
 *    @date  10/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "SAAPRunner.h"

#include "fwd/Tool.h"
#include "fwd/Filter.h"

#include "DBInterpreter.h"
#include "EventService.h"
#include "Event.h"
#include "Observer.h"

#include <list>
#include <memory>

SAAPRunner::SAAPRunner(std::unique_ptr<DBInterpreter>&& interpreter) :
    DBInterpreter_(std::move(interpreter)),
    ObserverList_(std::const_pointer_cast<ObserverList>(DBInterpreter_->eventService()->observerList()))
    {}

const ObserverList::const_iterator SAAPRunner::registerTool(std::unique_ptr<Tool>&& tool,
                                                      std::unique_ptr<Filter>&& filter,
                                                      Events&& events) const
{
    return ObserverList_->emplace(ObserverList_->cend(),
                                  Observer(std::move(tool), std::move(filter), std::move(events)));
}

void SAAPRunner::removeTool(ObserverList::const_iterator tool) const noexcept
{
    ObserverList_->erase(tool);
}

void SAAPRunner::interpret(const std::string& DBPath) const
{
    DBInterpreter_->process(DBPath);
}
