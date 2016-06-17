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

//******************************************************************************
// * Decoratable
// *****************************************************************************/
//class Decoratable {
//public:
//	virtual ~Decoratable() = 0;
//};
//inline Decoratable::~Decoratable(){}
//
//******************************************************************************
// * Decoration
// *****************************************************************************/
//template<typename Key, typename Value>
//class Decoration {
//public:
//
//	Decoration(const Value& initial) : kInitial_(initial) {};
//	~Decoration() {}
//
//	const Value& get(const Key& key) const;
//	void set(const Key& key, const Value& value);
//				   
//private:
//	typedef std::map<const Key&, const Value&> Map_;
//
//	Map_ map_;
//	const Value kInitial_;
//													 
//	// prevent generated functions
//	Decoration(const Decoration&);
//	Decoration& operator=(const Decoration&);
//};

/******************************************************************************
 * Abstract Event
 *****************************************************************************/
class Event {
public:
	Event(const ShadowThread *thread) : _thread(thread) {}
	virtual ~Event() {};

	const ShadowThread* getThread() const { return _thread; }
	virtual Events getEventType() const = 0;

private:
	const ShadowThread *_thread;
};

/******************************************************************************
 * New Thread Event
 *****************************************************************************/
struct NewThreadInfo {
  ShadowThread* childThread;
  ShadowThread* parentThread;
  NUM_CYCLES runtime;
  TIME startTime;
  NewThreadInfo(ShadowThread* childThread,
                ShadowThread* parentThread,
                NUM_CYCLES runtime,
                TIME_STRING startTimeString)
    : childThread(childThread), parentThread(parentThread),
	  runtime(runtime), startTime(timeStringToTime(startTimeString)) {}
};

class NewThreadEvent : public Event {
public:
	NewThreadEvent(const ShadowThread *thread,
				   const struct NewThreadInfo *info) :
					   Event(thread), _info(info) {}
	Events getEventType() const override { return Events::NEWTHREAD; }
	const NewThreadInfo* getNewThreadInfo() const {return _info; };

private:
	const NewThreadInfo *_info;

	// prevent generated functions
    NewThreadEvent(const NewThreadEvent&);
	NewThreadEvent& operator=(const NewThreadEvent&);
};


/******************************************************************************
 * Join Event
 *****************************************************************************/
struct JoinInfo {
  ShadowThread* childThread;
  ShadowThread* parentThread;
  JoinInfo(ShadowThread* childThread, ShadowThread* parentThread)
    : childThread(childThread), parentThread(parentThread) {}
};

class JoinEvent : public Event {
public:
	JoinEvent(const ShadowThread *thread,
			  const struct JoinInfo *info) :
			  Event(thread), _info(info) {}
	Events getEventType() const override { return Events::JOIN; }
	const JoinInfo* getJoinInfo() const { return _info; };

private:
	const JoinInfo *_info;

	// prevent generated functions
	JoinEvent(const JoinEvent&);
	JoinEvent& operator=(const JoinEvent&);
};

/******************************************************************************
 * Acquire Event
 *****************************************************************************/
struct AcquireInfo {
	ShadowLock *lock;
	AcquireInfo(ShadowLock *lock) : lock(lock) {}
};

class AcquireEvent : public Event {
public:
	AcquireEvent(const ShadowThread *thread,
				 const struct AcquireInfo *info) :
					 Event(thread), _info(info) {}
	Events getEventType() const override { return Events::ACQUIRE; }
	const AcquireInfo* getAcquireInfo() const { return _info; };

private:
	const struct AcquireInfo *_info;

	// prevent generated functions
	AcquireEvent(const AcquireEvent&);
	AcquireEvent& operator=(const AcquireEvent&);
};

/******************************************************************************
 * Release Event
 *****************************************************************************/
struct ReleaseInfo {
	ShadowLock *lock;
	ReleaseInfo(ShadowLock* lock) : lock(lock) {}
};

class ReleaseEvent : public Event {
public:
	ReleaseEvent(const ShadowThread *thread,
				 const struct ReleaseInfo *info) :
					 Event(thread), _info(info) {}

	Events getEventType() const override { return Events::RELEASE; }
	const ReleaseInfo* getReleaseInfo() const { return _info; };

private:
	const struct ReleaseInfo *_info;

	// prevent generated functions
	ReleaseEvent(const ReleaseEvent&);
	ReleaseEvent& operator=(const ReleaseEvent&);
};

/******************************************************************************
 * Access Event
 *****************************************************************************/
struct AccessInfo {
	AccessInfo( AccessType Type, ShadowVar *Var, INS_ID instructionID)
		: type(Type), instructionID(instructionID), var(Var) {}

	AccessType type;
	INS_ID instructionID;
	ShadowVar *var;
};

class AccessEvent : public Event {
public:
	AccessEvent(const ShadowThread *thread,
				const struct AccessInfo *info) :
					 Event(thread), _info(info) {}

	Events getEventType() const override { return Events::ACCESS; }
	const AccessInfo *getAccessInfo() const { return _info; };

private:
	const struct AccessInfo *_info;

	// prevent generated functions
	AccessEvent(const AccessEvent&);
	AccessEvent& operator=(const AccessEvent&);
};

/******************************************************************************
 * Call Event
 *****************************************************************************/
typedef struct CallInfo {
  CALLSITE siteId;
	TIME runtime;
  FUN_SG fnSignature;
	SEG_ID segment;
	FunctionType fnType;
	FIL_PT fileName;
  FIL_PT filePath;

    explicit
  CallInfo(CALLSITE SiteId,
           TIME Runtime,
           FUN_SG FnSignature,
           SEG_ID Segment,
           FunctionType FnType,
           FIL_PT FileName,
           FIL_PT FilePath)
    : siteId(SiteId), runtime(Runtime), fnSignature(FnSignature),
      segment(Segment), fnType(FnType), fileName(FileName), filePath(FilePath) {}
} CallInfo;

class CallEvent : public Event {
public:
	CallEvent(const ShadowThread *thread,
			  const struct CallInfo *info) :
					 Event(thread), _info(info) {}
	Events getEventType() const override { return Events::CALL; }
	const CallInfo* getCallInfo() const;

private:
	const struct CallInfo *_info;

	// prevent generated functions
	CallEvent(const CallEvent&);
	CallEvent& operator=(const CallEvent&);
};

typedef struct ReturnInfo {
	CAL_ID call;
	TIME   endTime;

	ReturnInfo(CAL_ID call,
			   TIME endTime)
		: call(call), endTime(endTime)
	{}
} ReturnInfo;

class ReturnEvent : public Event {
	public:
		ReturnEvent(const ShadowThread *thread,
				    const ReturnInfo *info) :
			Event(thread), _info(info) {}

		Events getEventType() const override { return Events::RETURN; }
		const ReturnInfo* getReturnInfo() const { return _info; };
	private:
		const ReturnInfo *_info;
};

#endif /* EVENT_H_ */
