/**
 *
 *    @file  AccessEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_EVENT_H_
#define  ACCESS_EVENT_H_

#include "fwd/AccessInfo.h"
#include "fwd/ShadowThread.h"

#include "Event.h"

#include <memory>

/// @ingroup events
/// @brief Event for memory access.
class AccessEvent final : public Event<AccessInfo> {
public:
    /// @brief Constructor.
    /// @param thread The thread the event was triggered from.
    /// @param info   The access event information.
    explicit AccessEvent(std::shared_ptr<const ShadowThread> thread,
                         std::unique_ptr<const AccessInfo>&& info)
        : Event(thread, std::move(info))
    {};

    /// _Default_ destructor.
    ~AccessEvent() = default;

    /// Return the event type.
    Events type() const final
    { return Events::ACCESS; };
};


#endif
