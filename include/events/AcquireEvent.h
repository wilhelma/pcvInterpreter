/**
 *
 *    @file  AcquireEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACQUIRE_EVENT_H_
#define  ACQUIRE_EVENT_H_

#include "fwd/AcquireInfo.h"
#include "fwd/ShadowThreadMap.h"

#include "Event.h"
#include "Types.h"

#include <memory>

/// @ingroup events
/// @brief Acquisition of a memory lock.
class AcquireEvent final : public Event<AcquireInfo> {
public:
    /// @brief Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info         The acquire event information.
    explicit AcquireEvent(ShadowThreadMap::const_iterator shadow_thread,
                          std::unique_ptr<const AcquireInfo>&& info)
        : Event(shadow_thread, std::move(info))
    {};

    /// Return the event type.
    Events type() const final
    { return Events::ACQUIRE; };
};

#endif
