/*
 * ParasiteTool.cpp
 
 *  Created on: May 9, 2016
 *      Author: knapp
 */

#include "ParasiteTool.h"

// Helper functions

CALLSITE  ParasiteTool::getCurrentCallSite() {

	return currentCallSiteID;
}

ShadowThread* ParasiteTool::getCurrentThread() {

	return currentThread;
}

FUN_SG ParasiteTool::getCurrentFunctionSignature(){

	return currentFunctionSignature;
}

void ParasiteTool::print()
{
 	print_parallelism_data(main_stack);
}

ParasiteTool::ParasiteTool() {

	parasite_stack_init(main_stack, MAIN);
}


ParasiteTool::~ParasiteTool() {

    this->print();
    destroy_stack(main_stack);
}


void ParasiteTool::create(const Event* e) {

	NewThreadEvent* newThreadEvent = (NewThreadEvent *) e;
	const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();

	currentThreadId = _info->childThread->threadId;
	time_t create_time = _info->runtime;

	thread_stack.push(currentThreadId);

	create_thread_operations(main_stack, create_time);
}

// this is a SYNC EVENT 
void ParasiteTool::join(const Event* e) {

	JoinEvent* joinEvent = (JoinEvent*) e;
	const JoinInfo *_info = joinEvent->getJoinInfo();
	childThreadId = _info->childThread->threadId;
	parentThreadId = _info->parentThread->threadId;

	assert(parentThreadId == thread_stack->bottom->threadId);

	thread_stack.pop();

	currentThreadId = parentThreadId;

	join_operations(main_stack);
}

void ParasiteTool::call(const Event* e) {

	CallEvent* callEvent = (CallEvent*) e;
	const CallInfo *_info = callEvent->getCallInfo();

	FUN_SG calledFunctionSignature = _info->fnSignature;
	TRD_ID calledThreadID = _info->getThread()->threadId;
	CALLSITE calledSiteID = _info->siteId;
	TIME callTime = _info->runtime;

	bottomFunctionSignature = main_stack->function_stack->bottom->functionSignature;

	if (calledFunctionSignature != bottomFunctionSignature) {

		return_of_called_operations(main_stack, return_time, last_strand_start);
	}

	currentFunctionSignature = calledFunctionSignature;

	bottomThreadID = main_stack->function_stack->bottom->functionSignature;

	if (calledThreadID != bottomThreadID) {

		thread_end_operations(main_stack, callTime, last_strand_start)
	}

	currenThreadID = calledThreadID;

	if (calledSiteID != currentCallSiteID)
		currentCallSiteID = calledSiteID;
	
    call_operations(main_stack, calledSiteID, callTime, last_strand_start, min_capacity);
}

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {

	AcquireEvent* acquireEvent = (AcquireEvent*) e;
	const AcquireInfo *_info = acquireEvent->getAcquireInfo();
	ShadowLock *acquiredLock = _info->lock;

	acquiredLock->last_acquire_time = e->runtime;
	lock_acquire_operations(main_stack);
}

// lock release event
void ParasiteTool::release(const Event* e) {

	ReleaseEvent* releaseEvent = (ReleaseEvent*) e;
	const ReleaseInfo *_info = releaseEvent->getReleaseInfo();
	ShadowLock *releasedLock = _info->lock;

	double lock_span = e->runtime - releasedLock->last_acquire_time;
	lock_release_operations(main_stack, lock_span);
}



