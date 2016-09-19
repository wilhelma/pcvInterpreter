/*
 * Tool.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef TOOL_H_
#define TOOL_H_

#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/CallEvent.h"
#include "fwd/JoinEvent.h"
#include "fwd/NewThreadEvent.h"
#include "fwd/ReleaseEvent.h"
#include "fwd/ReturnEvent.h"
#include "fwd/ThreadEndEvent.h"

/// @defgroup observers
/// @brief Observers

/// @ingroup observers
/// @brief Pure-virtual base class for every observer.
/// @attention This class is not copiable nor movable. So its derived
/// classes will be!
class Tool {
public:
    /// (Interface to) process AccessEvent.
    virtual void Access(const AccessEvent* e)       = 0;
    /// (Interface to) process AcquireEvent.
    virtual void Acquire(const AcquireEvent* e)     = 0;
    /// (Interface to) process CallEvent.
    virtual void Call(const CallEvent* e)           = 0;
    /// (Interface to) process JoinEvent.
    virtual void Join(const JoinEvent* e)           = 0;
    /// (Interface to) process NewThreadEvent.
    virtual void NewThread(const NewThreadEvent* e) = 0;
    /// (Interface to) process ReleaseEvent.
    virtual void Release(const ReleaseEvent* e)     = 0;
    /// (Interface to) process ReturnEvent.
    virtual void Return(const ReturnEvent* e)       = 0;
    /// (Interface to) process ThreadEndEvent.
    virtual void ThreadEnd(const ThreadEndEvent* e) = 0;

    /// _Default_ constructor.
    explicit Tool() = default;
    /// _Default_ destructor.
    virtual ~Tool() = default;

    /// _Deleted_ copy constructor.
    Tool(const Tool&) = delete;
    /// _Deleted_ move constructor.
    Tool(Tool&&)      = delete;

    /// _Deleted_ copy assignment operator.
    Tool& operator=(const Tool&) = delete;
    /// _Deleted_ move assignment operator.
    Tool& operator=(Tool&&)      = delete;
};

#endif
