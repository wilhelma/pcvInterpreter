/*
 * ParasiteTool.cpp
 
 *  Created on: May 9, 2016
 *      Author: knapp
 */

#define THREADS 100

#include <vector>
#include "ParasiteTool.h"
#include <iostream>
#include <array>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include "Interpreter.h"
#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "DataModel.h"
#include "DBDataModel.h"
#include "CilkProf.h"


const FUN_SG ParasiteTool::getSignature(TRD_ID id) {

	return threadFunctionMap[id];
}

void ParasiteTool::before_start_of_program() {

	// pthread_enter_begin(void* this_fn, void* rip);
	// rip = Return address of this function 

	// QUESTION: what is this_fn? 
	pthread_enter_begin(this_fn, rip);
	pthread_enter_end();
}


void ParasiteTool::create(const Event* e) {

	// F spawns or calls G:

	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	NewThreadEvent* newThreadEvent = (NewThreadEvent *) e;
	const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
	const FUN_SG parentSignature = getSignature(currentThread->threadId);
	currentThread = _info->childThread;

	pthread_create_prepare();
	pthread_create_or_continue(in_continuation);

	// QUESTION: NEED INFO FOR NEW THREAD'S FUNCTION SIGNATURE
	// parasite->addWorkSpan(parentSignature, _info->fnSignature, 0.0, 0.0, 0.0, 0.0);
}

// this is a SYNC EVENT 
void ParasiteTool::join(const Event* e) {

	// F syncs
	// if F.c > F.l
	// 		F.p += F.c
	// else
	// 		F.p += F.l
	//		F.p += F.lock_span
	//		F.p -= F.longest_child_lock_span
	// F.c = 0
	// F.l = 0
	// F.longest_child_lock_span = 0

	const FUN_SG F_signature = threadFunctionMap[currentThread->threadId];

	pthread_sync_begin();
	pthread_sync_end();

}

void ParasiteTool::call(const Event* e) {

	// QUESTION: should this be different from the create event? 

	// F spawns or calls G:
	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	CallEvent* callEvent = (CallEvent*) e;
	const CallInfo *_info = callEvent->getCallInfo();
	const FUN_SG parentSignature = getSignature(currentThread->threadId);
	parasite->addWorkSpan(parentSignature, _info->fnSignature, 0.0, 0.0, 0.0, 0.0);

	pthread_tool_c_function_enter(this_fn, rip);
}


// memory access event - the only related event to returns
void ParasiteTool::access(const Event* e) {

	AccessEvent* accessEvent = (AccessEvent*) e;
	const AccessInfo *_info = accessEvent->getAccessInfo();
	const FUN_SG F_signature = threadFunctionMap[currentThread->threadId];
}

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {


	AcquireEvent* acquireEvent = (AcquireEvent*) e;
	const AcquireInfo *_info = acquireEvent->getAcquireInfo();
	const FUN_SG F_signature = threadFunctionMap[currentThread->threadId];
}

// lock release event: IMPORTANT
void ParasiteTool::release(const Event* e) {

	ReleaseEvent* releaseEvent = (ReleaseEvent*) e;
	const ReleaseInfo *_info = releaseEvent->getReleaseInfo();
	const FUN_SG F_signature = threadFunctionMap[currentThread->threadId];

	// double lock_span = e->runtime - getLastLockStart(F_signature);
}

void ParasiteTool::after_end_of_program() {

	pthread_detach_begin();
	pthread_detach_end();
}


// NOT YET IMPLEMENTED IN PCVINTERPRETER
void ParasiteTool::returnOfCalled(const Event* e){

	// Called G returns to F:
	// G.p += G.c
	// F.w += G.w
	// F.c += G.p
    // F.lock_span += G.lock_span

	returnOfCalledInfo *_info = e->getJoinInfo();
	ShadowThread* childThread = _info->childThread;

	const FUN_SG F_signature = this.threadFunctionMap[currentThread->threadId];
	const FUN_SG G_signature = this.threadFunctionMap[childThread.threadId];

	pthread_tool_c_function_leave(rip);
}


// NOT YET IMPLEMENTED IN PCVINTERPRETER
void ParasiteTool::threadEnd(const Event* e){

	// Created G returns to F
	// G.p += G.c
	// F.w += G.w
	// F.lock_span += G.lock_span
	// if F.c + G.p > F.l
	// 		F.l = G.p
	//		F.longest_child_lock_span = G.lock_span 
	// 		F.p += G.c
	// 		F.c = 0

	ThreadEndInfo *_info = e->getJoinInfo();
	ShadowThread* childThread = _info->childThread;

	const FUN_SG F_signature = this.threadFunctionMap[currentThread->threadId];
	const FUN_SG G_signature = this.threadFunctionMap[childThread.threadId];

	pthread_leave_begin();
	pthread_leave_end();
}

