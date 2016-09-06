/**
 *
 *    @file  Observer.h
 *   @brief  
 *
 *    @date  09/04/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  OBSERVER_H_
#define  OBSERVER_H_

#include "Event.h"
#include "Filter.h"
#include "Tool.h"

#include <memory>

/// A compound of a Tool, a Filter and the Events the Tool
/// registers to.
struct Observer {
    /// The observing tool.
    std::unique_ptr<Tool> tool;
    /// The filter.
    std::unique_ptr<Filter> filter;
    /// The events the tool is registered to.
    Events events;

    /// Constructor.
    /// @param t Pointer to the Tool to register.
    /// @param f Pointer to the Filter
    /// @param e The events the Tool registers to.
    explicit Observer(std::unique_ptr<Tool>&& t,
                      std::unique_ptr<Filter>&& f,
                      Events&& e) noexcept :
        tool(std::move(t)),
        filter(std::move(f)),
        events(std::move(e))
        {}

    /// _Default_ destructor.
    ~Observer() = default;

    /// _Deleted_ copy constructor.
    Observer(const Observer&)            = delete;
    /// _Default move constructor.
    Observer(Observer&&)                 = default;
    /// _Deleted_ copy assignment operator.
    Observer& operator=(const Observer&) = delete;
    /// _Deleted_ move assignment operator.
    Observer& operator=(Observer&&)      = delete;
};

#endif
