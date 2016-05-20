/*
 * ParasiteTool.h
 
 *  Created on: May 9, 2016
 *      Author: knapp
 */

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
#define THREADS 100

ParasiteTool::ParasiteTool() {

	this->Parasite = new Parasite;
	this->threadFunctionMap = new threadFunctionMap;
}	

ParasiteTool::~ParasiteTool() {

	delete this.Parasite;
	delete this.threadFunctionMap;
}

void ParasiteTool::create(const Event* e) {

	// F spawns or calls G:

	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	CallInfo *_info = e->getCallInfo();
	this->Parasite.addFunctionWorkSpan(_info->fnSignature, 0.0, 0.0, 0.0, 0.0);
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

  

	const char* F_signature = this.threadFunctionMap[currentThread.threadId];

	if (Parasite->getContinuation(F_signature) > Parasite->getLongestChild(F_signature))
			Parasite.addToPrefix(F_signature, Parasite->getContinuation(F_signature));
	else {	
			Parasite.addToPrefix(F_signature, Parasite->getLongestChild(F_signature));
			Parasite.addToPrefix(F_signature, Parasite->getLockSpan(F_signature));
			Parasite.addToPrefix(F_signature, (0.0 - Parasite->getLockSpan(F_signature)));
	}

	setContinuation(F_signature, 0.0);
	setLongestChild(F_signature, 0.0);
}

void ParasiteTool::call(const Event* e) {

	// F spawns or calls G:
	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	CallInfo *_info = e->getCallInfo();
	ShadowThread* childThread = _info->childThread;
	const char* G_signature = this.threadFunctionMap[childThread.threadId];
	Parasite.setWorkSpan(_info->fnSignature, 0.0, 0.0, 0.0, 0.0);
}


// memory access event - the only related event to returns
void ParasiteTool::access(const Event* e) {

	AccessInfo *_info = e->getAccessInfo();
	const char* F_signature = this.threadFunctionMap[currentThread.threadId];

}

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {

	AcquireInfo *_info = e->getAcquireInfo();
	const char* F_signature = this.threadFunctionMap[currentThread.threadId];
	// double last_lock_start = e->runtime;
	double last_lock_start = 0.0;
	Parasite->setLastLockStart(F_signature, last_lock_start);
}

// lock release event: IMPORTANT
void ParasiteTool::release(const Event* e) {

	ReleaseInfo *_info = e->getReleaseInfo();
	const char* F_signature = this.threadFunctionMap[currentThread.threadId];
	// double lock_span = e->runtime - getLastLockStart(F_signature);
	double lock_span = 10.0;
	Parasite->addToLockSpan(F_signature, lock_span);
}

// NOT YET IMPLEMENTED IN PCVINTERPRETER
void ParasiteTool::returnOfCalled(const Event* e){

	// Called G returns to F:
	// G.p += G.c
	// F.w += G.w
	// F.c += G.p

	returnOfCalledInfo *_info = e->getJoinInfo();
	ShadowThread* childThread = _info->childThread;

	const char* F_signature = this.threadFunctionMap[currentThread.threadId];
	const char* G_signature = this.threadFunctionMap[childThread.threadId];

	G_w = 42.0;
	// G_w = _info->runtime;
	Parasite->setWork(G_signature, G_w);

	Parasite.addToPrefix(G_signature, Parasite->getContinuation(G_signature));
	Parasite.addToWork(F_signature, Parasite->getWork(G_signature));
	Parasite.addToContinuation(F_signature, Parasite->getPrefix(G_signature));
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

	const char* F_signature = this.threadFunctionMap[currentThread.threadId];
	const char* G_signature = this.threadFunctionMap[childThread.threadId];

	G_w = 42.0;
	// G_w = _info->runtime;
	Parasite->setWork(G_signature, G_w);

	G_c = Parasite->getContinuation(G_signature);
	G_p = Parasite->getPrefix(G_signature);
	G_ls = Parasite->getLockSpan(G_signature);

	Parasite.addToPrefix(G_signature, G_c);
	Parasite.addToWork(F_signature, Parasite->getWork(G_signature));
	Parasite.addToLockSpan(F_signature, G_ls);


	if ((Parasite->getContinuation(F_signature) + G_p) > Parasite->getLongestChild(F_signature)) {

		Parasite.setLongestChild(F_signature, G_p);
		Parasite.setLongestChildLockSpan(F_signature, G_ls);
		Parasite.addToPrefix(F_signature, G_c);
		Parasite.setContinuation(F_signature, 0.0);
	}
}

