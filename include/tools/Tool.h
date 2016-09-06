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

class Tool {

public:

virtual void Access(const AccessEvent* e) = 0;
virtual void Acquire(const AcquireEvent* e) = 0;
virtual void Call(const CallEvent* e) = 0;
virtual void Join(const JoinEvent* e) = 0;
virtual void NewThread(const NewThreadEvent* e) = 0;
virtual void Release(const ReleaseEvent* e) = 0;
virtual void Return(const ReturnEvent* e) = 0;
virtual void ThreadEnd(const ThreadEndEvent* e) = 0;
virtual ~Tool() {};

};

#endif
