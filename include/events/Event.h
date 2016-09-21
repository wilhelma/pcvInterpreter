/// @file Event.h

#ifndef EVENT_H_
#define EVENT_H_

#include "fwd/ShadowThread.h"

#include "Types.h"

#include <string>
#include <stdlib.h>

/// @brief Run-time events.
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

///// Converts a time in the format `YYYY-MM-DD|T|hh:mm:ss|.|CLOCK|Z| into 
///// a `TIME` variable (i.e. `clock_t`)
//inline
//const TIME timeStringToTime(const TIME_STRING& t) {
//	int found = t.find_last_of('.');
//	return static_cast<TIME>(atoi(t.substr(found + 1).c_str()));
//}

/// @brief Abstract event.
/// @details Base class for all the `*Event` classes.
/// @attention Event is not copyable nor moveable! So its inherited classes will be.
template <typename InfoType>
class Event {
public:
	/// Constructor.
    /// @param  thread The thread the event was triggered from.
    /// @tparam info   The event information.
	Event(const ShadowThread* thread, const InfoType* info) :
		Thread_(thread), Info_(info) {};

	/// _Default_ destructor.
	virtual ~Event() = default;

    /// _Deleted_ copy constructor.
    Event(const Event&)            = delete;
    /// _Deleted_ move constructor.
    Event(Event&&)                 = delete;
    /// _Deleted_ copy assignment operator.
    Event& operator=(const Event&) = delete;
    /// _Deleted_ move assignment operator.
    Event& operator=(Event&&)      = delete;

    /// Acesses the thread information.
	decltype(auto) getThread() const
    { return Thread_; };

    /// Access the event information.
    decltype(auto) getInfo() const
    { return Info_; };

    /// Returns the event type.
	virtual Events getEventType() const = 0;

private:
	/// @todo Should this be a `unique_ptr`?
	const ShadowThread* const Thread_;

    /// The event information.
    const InfoType* const Info_;
};

#endif /* EVENT_H_ */
