/// @file Event.h

#ifndef EVENT_H_
#define EVENT_H_

#include "fwd/ShadowThread.h"

#include "Types.h"

#include <string>
#include <stdlib.h>

/// @brief Run-time events.
enum class Events: unsigned char {
    NEWTHREAD = 0x1,  // 00000001
    THREADEND = 0x2,  // 00000010
    JOIN      = 0x4,  // 00000100
    ACQUIRE   = 0x8,  // 00001000
    RELEASE   = 0x10, // 00010000
    ACCESS    = 0x20, // 00100000
	CALL      = 0x40, // 01000000
    RETURN    = 0x80, // 10000000
    ALL       = 0xFF  // 11111111 
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

/// @brief Abstract Event.
/// @details Base class for all the `*Event` classes.
class Event {
public:
	/// Constructor.
	explicit Event(const ShadowThread *thread) noexcept :
		Thread_(thread) {};

	/// _Default_ destructor.
	virtual ~Event() = default;

	const ShadowThread* const getThread() const { return Thread_; };
	virtual Events getEventType() const = 0;

private:
	/// @todo Should this be a `unique_ptr`?
	const ShadowThread* const Thread_;
};

#endif /* EVENT_H_ */
