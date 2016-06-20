#ifndef EVENT_H_
#define EVENT_H_

#include <cstring>
#include <map>
#include <iostream>
#include "DataModel.h"

// to retrieve the custom typedefs
#include "Types.h"

class ShadowThread;
class ShadowLock;
class ShadowVar;

enum class Events: unsigned char {
    NEWTHREAD = 0x1,  // 00000001
    JOIN      = 0x2,  // 00000010
    ACQUIRE   = 0x4,  // 00000100
    RELEASE   = 0x8,  // 00001000
    ACCESS    = 0x10, // 00010000
    CALL      = 0x20, // 00100000
	RETURN    = 0x40, // 01000000
    ALL       = 0xFF  // 11111111
};

constexpr inline
Events operator| (Events A, Events B) noexcept {
    return static_cast<Events>(
        static_cast<std::underlying_type<Events>::type>(A) |
        static_cast<std::underlying_type<Events>::type>(B)   );
}

template <typename T>
constexpr inline
bool operator== (Events A, T b ) noexcept {
  return static_cast<bool>(
        static_cast<T>(
          static_cast<std::underlying_type<Events>::type>(A)) == b );
}

template <typename T>
constexpr inline
bool operator!= (Events A, T b ) noexcept {
	return !( A == b );
}

/// Converts a time in the format `YYYY-MM-DD|T|hh:mm:ss|.|CLOCK|Z| into 
/// a `TIME` variable (i.e. `clock_t`)
inline
const TIME timeStringToTime(const TIME_STRING& t) {
	int found = t.find_last_of('.');
	return static_cast<TIME>(atoi(t.substr(found + 1).c_str()));
}



/// Abstract Event
class Event {
public:
	Event(const ShadowThread *thread) : _thread(thread) {}
	virtual ~Event() {};

	const ShadowThread* getThread() const { return _thread; }
	virtual Events getEventType() const = 0;

private:
	const ShadowThread *_thread;
};

#endif /* EVENT_H_ */
