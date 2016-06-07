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

TRD_ID ParasiteTool::getCurrentThreadID() {

	return currentThreadID;
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

	currentThreadID = _info->childThread->threadId;

	// TIME create_time = _info->runtime;

	TIME create_time = (TIME) 1;

	thread_push(main_stack);

	create_thread_operations(main_stack, last_strand_start, create_time);
}

// this is a SYNC EVENT 
void ParasiteTool::join(const Event* e) {

	JoinEvent* joinEvent = (JoinEvent*) e;
	const JoinInfo *_info = joinEvent->getJoinInfo();
	TRD_ID childThreadId = _info->childThread->threadId;
	TRD_ID parentThreadId = _info->parentThread->threadId;

	assert(parentThreadId == main_stack->thread_stack[main_stack->thread_stack_tail_index].threadId);

	thread_pop(main_stack);

	currentThreadID = parentThreadId;

	TIME join_time = (TIME) 1;

	join_operations(main_stack, last_strand_start, join_time, min_capacity);
}

void ParasiteTool::call(const Event* e) {

	CallEvent* callEvent = (CallEvent*) e;
	const CallInfo *_info = callEvent->getCallInfo();

	FUN_SG calledFunctionSignature = _info->fnSignature;
	TRD_ID calledThreadID = callEvent->getThread()->threadId;
	CALLSITE calledSiteID = _info->siteId;
	TIME callTime = _info->runtime;
	TIME returnTime = _info->runtime;

	FUN_SG bottomFunctionSignature = main_stack->function_stack[main_stack->function_stack_tail_index].functionSignature;

	if (calledFunctionSignature != bottomFunctionSignature) {

		return_of_called_operations(main_stack, returnTime, last_strand_start);
	}

	currentFunctionSignature = calledFunctionSignature;

	TRD_ID bottomThreadID = main_stack->thread_stack[main_stack->thread_stack_tail_index].threadId;

	if (calledThreadID != bottomThreadID) {

		thread_end_operations(main_stack, callTime, last_strand_start);
	}

	currentThreadID = calledThreadID;

	if (calledSiteID != currentCallSiteID)
		currentCallSiteID = calledSiteID;
	
    call_operations(main_stack, calledSiteID, callTime, last_strand_start, min_capacity);
}

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {

	// AcquireEvent* acquireEvent = (AcquireEvent*) e;
	// const AcquireInfo *_info = acquireEvent->getAcquireInfo();
	// ShadowLock *acquiredLock = _info->lock;

	// acquiredLock->last_acquire_time = e->runtime;
	// lock_acquire_operations(main_stack);
}

// lock release event
void ParasiteTool::release(const Event* e) {

	// ReleaseEvent* releaseEvent = (ReleaseEvent*) e;
	// const ReleaseInfo *_info = releaseEvent->getReleaseInfo();
	// ShadowLock *releasedLock = _info->lock;

	// double lock_span = e->runtime - releasedLock->last_acquire_time;
	// lock_release_operations(main_stack, lock_span);
}



