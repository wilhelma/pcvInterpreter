#ifndef EVENT_H_
#define EVENT_H_

class ShadowThread;
class ShadowLock;
class ShadowVar;


enum Events{
    NEWTHREAD = 0x1,
    ACQUIRE = 0x2,
    RELEASE = 0x4,
    ACCESS = 0x8,
    CALL = 0x10,
    ALL = 0xFF
};

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
class NewThreadEvent : public Event {
public:
	NewThreadEvent(const ShadowThread *thread,
				   const ShadowThread *newThread) :
					   Event(thread), _newThread(newThread) {}
	Events getEventType() const override { return NEWTHREAD; }
	const ShadowThread* getNewThread() const;

private:
	const ShadowThread *_newThread;

	// prevent generated functions
	NewThreadEvent(const NewThreadEvent&);
	NewThreadEvent& operator=(const NewThreadEvent&);
};

/******************************************************************************
 * Acquire Event
 *****************************************************************************/
struct AcquireInfo {
	ShadowLock *lock;
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
};

class ReleaseEvent : public Event {
public:
	ReleaseEvent(const ShadowThread *thread,
				 const struct ReleaseInfo *info) :
					 Event(thread), _info(info) {}
	Events getEventType() const override { return RELEASE; }

private:
	const struct ReleaseInfo *_info;

	// prevent generated functions
	ReleaseEvent(const ReleaseEvent&);
	ReleaseEvent& operator=(const ReleaseEvent&);
};

/******************************************************************************
 * Access Event
 *****************************************************************************/
enum AccessType {
	READ,
	WRITE,
	READWRITE
};

struct AccessInfo {
	enum AccessType type;
	const ShadowVar *var;
};

class AccessEvent : public Event {
public:
	AccessEvent(const ShadowThread *thread,
				const struct AccessInfo *info) :
					 Event(thread), _info(info) {}
	Events getEventType() const override { return ACCESS; }
	const ShadowVar *getAccessInfo() const;

private:
	const struct AccessInfo *_info;


	// prevent generated functions
	AccessEvent(const AccessEvent&);
	AccessEvent& operator=(const AccessEvent&);
};

/******************************************************************************
 * Call Event
 *****************************************************************************/
struct CallInfo {
	const char* name;
};

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
