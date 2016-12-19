/**
 *
 *    @file  EventType.h
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  EVENT_TYPE_H_
#define  EVENT_TYPE_H_

#include <ostream>
#include <type_traits>

/// @ingroup events
/// @brief Run-time event types.
enum class EventType : unsigned char {
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

/// @brief Bitwise _and_ operator for `EventType`.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
constexpr inline
EventType operator&(EventType lhs, EventType rhs) noexcept {
    return static_cast<EventType>(
        static_cast<std::underlying_type<EventType>::type>(lhs) &
        static_cast<std::underlying_type<EventType>::type>(rhs)   );
}

/// @brief Output stream operator for EventType.
inline std::ostream& operator<<(std::ostream& s, EventType e) {
    return s << static_cast<std::underlying_type<EventType>::type>(e);
}

#endif
