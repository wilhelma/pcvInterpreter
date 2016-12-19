/**
 *
 *    @file  NewThreadEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  NEW_THREAD_EVENT_H_
#define  NEW_THREAD_EVENT_H_

#include "fwd/EventType.h"
#include "fwd/NewThreadInfo.h"
#include "fwd/ShadowThreadMap.h"

#include "Event.h"
#include "Types.h"

#include <iosfwd>
#include <memory>

/// @ingroup events
/// @brief Event for a new thread creation.
class NewThreadEvent final : public Event<NewThreadInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The new thread event information.
    explicit NewThreadEvent(ShadowThreadMap::const_iterator shadow_thread,
                            std::unique_ptr<const NewThreadInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {}

    /// Returns the event type.
    EventType type() const noexcept final;
};

/// @brief Output stream operator for NewThreadEvent.
/// @param s The stream.
/// @param e The event to stram.
std::ostream& operator<<(std::ostream& s, const NewThreadEvent& e);

#endif
