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

void start_of_program() {



}


void ParasiteTool::create(const Event* e) {

	// QUESTION: should this be different than the call event? 

	// F spawns or calls G:

	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	NewThreadEvent* newThreadEvent = (NewThreadEvent *) e;
	const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
	const FUN_SG parentSignature = getSignature(currentThread->threadId);
	currentThread = _info->childThread;

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

	if (parasite->getContinuation(F_signature) > parasite->getLongestChild(F_signature))
			parasite->addToPrefix(F_signature, parasite->getContinuation(F_signature));
	else {	
			parasite->addToPrefix(F_signature, parasite->getLongestChild(F_signature));
			parasite->addToPrefix(F_signature, parasite->getLockSpan(F_signature));
			parasite->addToPrefix(F_signature, (0.0 - parasite->getLockSpan(F_signature)));
	}

	parasite->setLongestChild(F_signature, 0.0);
	parasite->setLongestChild(F_signature, 0.0);
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
	// double last_lock_start = e->runtime;
	double last_lock_start = 0.0;
	parasite->setLastLockStart(F_signature, last_lock_start);
}

// lock release event: IMPORTANT
void ParasiteTool::release(const Event* e) {

	ReleaseEvent* releaseEvent = (ReleaseEvent*) e;
	const ReleaseInfo *_info = releaseEvent->getReleaseInfo();
	const FUN_SG F_signature = threadFunctionMap[currentThread->threadId];
	// double lock_span = e->runtime - getLastLockStart(F_signature);
	double lock_span = 10.0;
	parasite->addToLockSpan(F_signature, lock_span);
}

void end_of_program() {



}


// // NOT YET IMPLEMENTED IN PCVINTERPRETER
// void ParasiteTool::returnOfCalled(const Event* e){

// 	// Called G returns to F:
// 	// G.p += G.c
// 	// F.w += G.w
//	// F.c += G.p
//  // F.lock_span += G.lock_span

// 	returnOfCalledInfo *_info = e->getJoinInfo();
// 	ShadowThread* childThread = _info->childThread;

// 	const FUN_SG F_signature = this.threadFunctionMap[currentThread->threadId];
// 	const FUN_SG G_signature = this.threadFunctionMap[childThread.threadId];

// 	G_w = 42.0;
// 	// G_w = _info->runtime;
// 	parasite->setWork(G_signature, G_w);

// 	parasite->addToPrefix(G_signature, parasite->getContinuation(G_signature));
// 	parasite->addToWork(F_signature, parasite->getWork(G_signature));
// 	parasite->addToContinuation(F_signature, parasite->getPrefix(G_signature));
// }


// // NOT YET IMPLEMENTED IN PCVINTERPRETER
// void ParasiteTool::threadEnd(const Event* e){

// 	// Created G returns to F
// 	// G.p += G.c
// 	// F.w += G.w
// 	// F.lock_span += G.lock_span
// 	// if F.c + G.p > F.l
// 	// 		F.l = G.p
// 	//		F.longest_child_lock_span = G.lock_span 
// 	// 		F.p += G.c
// 	// 		F.c = 0

// 	ThreadEndInfo *_info = e->getJoinInfo();
// 	ShadowThread* childThread = _info->childThread;

// 	const FUN_SG F_signature = this.threadFunctionMap[currentThread->threadId];
// 	const FUN_SG G_signature = this.threadFunctionMap[childThread.threadId];

// 	G_w = 42.0;
// 	// G_w = _info->runtime;
// 	parasite->setWork(G_signature, G_w);

// 	G_c = parasite->getContinuation(G_signature);
// 	G_p = parasite->getPrefix(G_signature);
// 	G_ls = parasite->getLockSpan(G_signature);

// 	parasite->addToPrefix(G_signature, G_c);
// 	parasite->addToWork(F_signature, parasite->getWork(G_signature));
// 	parasite->addToLockSpan(F_signature, G_ls);


// 	if ((parasite->getContinuation(F_signature) + G_p) > parasite->getLongestChild(F_signature)) {

// 		parasite->setLongestChild(F_signature, G_p);
// 		parasite->setLongestChildLockSpan(F_signature, G_ls);
// 		parasite->addToPrefix(F_signature, G_c);
// 		parasite->parasite.setLongestChild(F_signature, 0.0);
// 	}
// }

