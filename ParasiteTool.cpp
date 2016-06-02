/*
 * ParasiteTool.cpp
 
 *  Created on: May 9, 2016
 *      Author: knapp
 */

#include "ParasiteTool.h"

// Helper functions

unsigned long long ParasiteTool::getCurrentCallSite() {

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

	currentThread = _info->childThread;

	// TIME create_time = _info->runtime;
	// create_thread_operations(main_stack);
}

// this is a SYNC EVENT 
void ParasiteTool::join(const Event* e) {

	JoinEvent* joinEvent = (JoinEvent*) e;
	const JoinInfo *_info = joinEvent->getJoinInfo();

	// TIME joinTime = 0; 
	// join_operations(main_stack, joinTime);
}

void ParasiteTool::call(const Event* e) {

	CallEvent* callEvent = (CallEvent*) e;
	const CallInfo *_info = callEvent->getCallInfo();

	currentFunctionSignature = _info->fnSignature;

	if (currentCallSiteID != _info->siteId)
		currentCallSiteID = _info->siteId;

    call_operations(main_stack, _info->siteId, _info->runtime);
}


// memory access event - the only related event to returns
void ParasiteTool::access(const Event* e) {

	AccessEvent* accessEvent = (AccessEvent*) e;
	const AccessInfo *_info = accessEvent->getAccessInfo();

	// memory_access_operations(main_stack);
}

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {

	AcquireEvent* acquireEvent = (AcquireEvent*) e;
	const AcquireInfo *_info = acquireEvent->getAcquireInfo();

	// last_lock_start = e->runtime;

	lock_acquire_operations(main_stack);
}

// lock release event: IMPORTANT
void ParasiteTool::release(const Event* e) {

	ReleaseEvent* releaseEvent = (ReleaseEvent*) e;
	const ReleaseInfo *_info = releaseEvent->getReleaseInfo();

	// double lock_span = e->runtime - getLastLockStart(F_signature);
	double lock_span = 10.0;

	lock_release_operations(main_stack);
}

// NOT YET IMPLEMENTED IN PCVINTERPRETER
void ParasiteTool::returnOfCalled(const Event* e){

	// ReturnOfCalledEvent* returnOfCalledEvent = (ReturnOfCalledEvent*) e;
	// returnOfCalledInfo *_info = e->getReturnOfCalledInfo();

	// return_of_called_operations(main_stack, return_time);
}

// NOT YET IMPLEMENTED IN PCVINTERPRETER
void ParasiteTool::threadEnd(const Event* e){

	// ThreadEndEvent* threadEndEvent = (ThreadEndEvent*) e;
	// threadEndInfo *_info = e->getThreadEndInfo();

	// thread_end_operations(main_stack, thread_end_time);
}

