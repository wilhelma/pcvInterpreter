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
#include "fwd/ShadowThreadMap.h"
#include <iosfwd>

#include "Event.h"
#include "Types.h"

#include <memory>

/// @ingroup events
/// @brief Event for memory access.
class AccessEvent final : public Event<AccessInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info          The access event information.
    explicit AccessEvent(ShadowThreadMap::const_iterator shadow_thread,
                         std::unique_ptr<const AccessInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {};

    /// _Default_ destructor.
    ~AccessEvent() = default;

    /// Return the event type.
    Events type() const final
    { return Events::ACCESS; };
};

/// Stream operator for AccessEvent.
std::ostream& operator<<(std::ostream& s, const AccessEvent& e);

#endif
