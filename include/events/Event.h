/// @file Event.h

#ifndef EVENT_H_
#define EVENT_H_

#include "ShadowThread.h"
#include "ShadowThreadMap.h"
#include "Types.h"

// -----------------------------------------------------
// Include these otherwise I get error from Parasite (?)
#include "AccessInfo.h"
#include "JoinInfo.h"
// -----------------------------------------------------

#include <memory>
#include <type_traits>

/// @defgroup events
/// @brief Runtime events 

/// @ingroup events
/// @brief Run-time event types.
enum class Events: unsigned char {
    NEWTHREAD = 0x1,  ///< A new thread has been created.
    THREADEND = 0x2,  ///< A thread terminates (i.e. returns).
    JOIN      = 0x4,  ///< Two threads are joint.
    ACQUIRE   = 0x8,  ///< A lock has been acquired.
    RELEASE   = 0x10, ///< A lock has been released.
    ACCESS    = 0x20, ///< A memory location has been accessed.
    CALL      = 0x40, ///< A function has been called.
    RETURN    = 0x80, ///< A function has returned.
    ALL       = 0xFF  ///< An event matches all the previous. 
};

/// @brief Bitwise _or_ operator for `Events`.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
constexpr inline
Events operator|(Events lhs, Events rhs) noexcept {
    return static_cast<Events>(
        static_cast<std::underlying_type<Events>::type>(lhs) |
        static_cast<std::underlying_type<Events>::type>(rhs)   );
}

/// @brief Bitwise _and_ operator for `Events`.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
constexpr inline
Events operator&(Events lhs, Events rhs) noexcept {
    return static_cast<Events>(
        static_cast<std::underlying_type<Events>::type>(lhs) &
        static_cast<std::underlying_type<Events>::type>(rhs)   );
}

/// @brief Equality operator for `Event` and a template type.
/// @attention The template type `T` has to support `operator==` with the
/// underlying type of `Events`.
/// @param  lhs The event.
/// @tparam rhs The right-hand-side operator to be compared with the event.
/// @bug   This shouldn't be needed! (only Events-Evenst comparison should be allowed!)
template <typename T>
constexpr inline
const bool operator==(Events lhs, T rhs) noexcept {
  return static_cast<bool>(
        static_cast<T>(static_cast<std::underlying_type<Events>::type>(lhs)) == rhs );
}

/// @brief Equality operator for `Event` (make it commutative).
/// @details Calls `operator==(Events, T)`.
/// @tparam lhs The right-hand-side operator to be compared with the event.
/// @param  rhs The event.
/// @bug   This shouldn't be needed! (only Events-Evenst comparison should be allowed!)
template <typename T>
constexpr inline
const bool operator==(T lhs, Events rhs) noexcept {
    return rhs == lhs;
}

/// @brief Unequality operator: negates `operator==(Events, T)`.
/// @bug   This shouldn't be needed! (only Events-Evenst comparison should be allowed!)
template <typename T>
constexpr inline
const bool operator!=(Events lhs, T rhs) noexcept {
    return !(lhs == rhs);
}

/// @brief Unequality operator (make it commutative).
/// @bug   This shouldn't be needed! (only Events-Evenst comparison should be allowed!)
template <typename T>
constexpr inline
const bool operator!=(T lhs, Events rhs) noexcept {
    return rhs != lhs;
}

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
    explicit Event(ShadowThreadMap::const_iterator shadow_thread,
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
    const TRD_ID& threadId() const
    { return Thread_->first; }

    /// Acesses the thread information.
    const std::shared_ptr<const ShadowThread>& thread() const
    { return Thread_->second; }

    /// Access the event information.
    const std::unique_ptr<const InfoType>& info() const
    { return Info_; }

    /// Returns the event type.
    virtual Events type() const = 0;

private:
    /// Pointer to the ShadowThread (shared among several events).
    ShadowThreadMap::const_iterator Thread_;

    /// The event information.
    const std::unique_ptr<const InfoType> Info_;
};

#endif
