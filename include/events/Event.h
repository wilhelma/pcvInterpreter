/// @file Event.h

#ifndef EVENT_H_
#define EVENT_H_

#include "fwd/EventType.h"
#include "fwd/ShadowThread.h"

#include "ShadowThreadMap.h"
#include "Types.h"

#include <memory>

/// @defgroup events
/// @brief Runtime events 

/// @ingroup events
/// @brief Abstract event.
/// @tparam The information type.
/// @details Base class for all the `*Event` classes.
template <typename InfoType>
class Event {
public:
    /// Constructor.
    /// @param shadow_thread The shadow information of the thread that triggered the event.
    /// @param info          The event information.
    constexpr explicit Event(ShadowThreadMap::const_iterator shadow_thread,
                             std::unique_ptr<const InfoType>&& info) noexcept
        : Thread_(shadow_thread),
          Info_(std::move(info))
    {}

    /// _Default_ destructor.
    virtual ~Event() = default;

    /// _Deleted_ copy constructor.
    Event(const Event&) = delete;
    /// _Deleted_ move constructor.
    Event(Event&&)      = delete;
    /// _Deleted_ copy assignment operator.
    Event& operator=(const Event&) = delete;
    /// _Deleted_ move assignment operator.
    Event& operator=(Event&&)      = delete;

    /// Acesses the thread ID.
    constexpr const TRD_ID& threadId() const noexcept
    { return Thread_->first; }

    /// Acesses the thread information.
    constexpr const std::shared_ptr<const ShadowThread>& thread() const noexcept
    { return Thread_->second; }

    /// Access the event information.
    constexpr const std::unique_ptr<const InfoType>& info() const noexcept
    { return Info_; }

    /// Returns the event type.
    virtual EventType type() const noexcept = 0;

private:
    /// Pointer to the ShadowThread (shared among several events).
    ShadowThreadMap::const_iterator Thread_;

    /// The event information.
    const std::unique_ptr<const InfoType> Info_;
};

#endif
