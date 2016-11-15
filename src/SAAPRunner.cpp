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
#include "Event.h"
#include "EventService.h"
#include "Observer.h"
#include "ShadowLockMap.h"
#include "ShadowThreadMap.h"

#include <list>
#include <memory>

// Helper function to create the DBInterpreter
std::unique_ptr<DBInterpreter> make_DBInterpreter(const std::shared_ptr<const ObserverList>& ol) {
    return std::make_unique<DBInterpreter>(
            std::make_unique<EventService>(ol),
            std::make_unique<ShadowLockMap>(),
            std::make_unique<ShadowThreadMap>());
}

SAAPRunner::SAAPRunner() :
    ObserverList_(std::make_shared<ObserverList>()),
    DBInterpreter_(make_DBInterpreter(ObserverList_))
    {}

SAAPRunner::~SAAPRunner() = default;

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
