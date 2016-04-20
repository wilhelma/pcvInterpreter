#ifndef EVENT_H_
#define EVENT_H_

#include <cstring>
#include <map>
#include "DataModel.h"

// to retrieve the custom typedefs
#include "Types.h"

class ShadowThread;
class ShadowLock;
class ShadowVar;

enum Events{
    NEWTHREAD = 0x1,
	JOIN = 0x2,
    ACQUIRE = 0x4,
    RELEASE = 0x8,
    ACCESS = 0x10,
    CALL = 0x20,
    ALL = 0xFF
};

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
	NewThreadInfo(ShadowThread* childThread)
		: childThread(childThread) {}
};

class NewThreadEvent : public Event {
public:
	NewThreadEvent(const ShadowThread *thread,
				   const struct NewThreadInfo *info) :
					   Event(thread), _info(info) {}
	Events getEventType() const override { return NEWTHREAD; }
	const NewThreadInfo* getNewThreadInfo() const;

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
	JoinInfo(ShadowThread* childThread) : childThread(childThread) {}
};

class JoinEvent : public Event {
public:
	JoinEvent(const ShadowThread *thread,
			  const struct JoinInfo *info) :
			  Event(thread), _info(info) {}
	Events getEventType() const override { return JOIN; }
	const JoinInfo* getJoinInfo() const;

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
	Events getEventType() const override { return ACQUIRE; }
	const AcquireInfo* getAcquireInfo() const;

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
	Events getEventType() const override { return RELEASE; }
	const ReleaseInfo* getReleaseInfo() const;

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
	AccessInfo( ACC_TYP Type, ShadowVar *Var, INS_ID instructionID)
		: type(Type), instructionID(instructionID), var(Var) {}

	ACC_TYP type;
	INS_ID instructionID;
	ShadowVar *var;
};

class AccessEvent : public Event {
public:
	AccessEvent(const ShadowThread *thread,
				const struct AccessInfo *info) :
					 Event(thread), _info(info) {}

	Events getEventType() const override { return ACCESS; }
	const AccessInfo *getAccessInfo() const;

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
	TIME runtime;
    FUN_SG fnSignature;
	SEG_ID segment;
	FUN_TYP fnType;
	FIL_PT fileName;
	FIL_PT filePath;

    explicit
	CallInfo(TIME Runtime,
			 FUN_SG FnSignature,
			 SEG_ID Segment,
			 FUN_TYP FnType,
			 FIL_PT FileName,
			 FIL_PT FilePath)
		: runtime(Runtime), fnSignature(FnSignature), segment(Segment), fnType(FnType),
		  fileName(FileName), filePath(FilePath) {}
} CallInfo;

class CallEvent : public Event {
public:
	CallEvent(const ShadowThread *thread,
			  const struct CallInfo *info) :
					 Event(thread), _info(info) {}
	Events getEventType() const override { return CALL; }
	const CallInfo* getCallInfo() const;

private:
	const struct CallInfo *_info;

	// prevent generated functions
	CallEvent(const CallEvent&);
	CallEvent& operator=(const CallEvent&);
};

#endif /* EVENT_H_ */
