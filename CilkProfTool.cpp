/*
 * CilkProfTool.h
 
 *  Created on: May 9, 2016
 *      Author: knapp
 */

#include <vector>
#include "CilkProfTool.h"
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


CilkProfTool::CilkProfTool() {

	this->cilkprof = new CilkProf;
	this->threadFunctionMap = new threadFunctionMap;
}	

CilkProfTool::~CilkProfTool() {

	delete this.cilkprof;
	delete this.threadFunctionMap;
}

void CilkProfTool::create(const Event* e) {

	// F spawns or calls G:

	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	CallInfo *_info = e->cilkprof.getCallInfo();
	this->cilkprof.addFunctionWorkSpan(_info->fnSignature, 0.0, 0.0, 0.0, 0.0);
}


void CilkProfTool::join(const Event* e) {

	// Spawned G returns to F
	// G.p += G.c
	// F.w += G.w
	// if F.c + G.p > F.l
	// 		F.l = G.p
	// 		F.p += G.c
	// 		F.c = 0

	JoinInfo *_info = e->cilkprof.getJoinInfo();
	ShadowThread* childThread = _info->childThread;

	const char* F_signature = this.threadFunctionMap[currentThread.threadId];
	const char* G_signature = this.threadFunctionMap[childThread.threadId];

	G_c = cilkprof.getContinuation(G_signature);

	this->cilkprof.addToPrefix(G_signature, G_c);
	this->cilkprof.addToWork(F_signature, cilkprof.getWork(G_signature));

	G_p = cilkprof.getPrefix(G_signature);

	if ((cilkprof.getContinuation(F_signature) + G_p) > cilkprof.getLongestChild(F_signature)) {

		setLongestChild(F_signature, G_p);
		cilkprof.addToPrefix(F_signature, G_c);
		setContinuation(F_signature, 0.0);
	}
}

// lock acquire event 
void CilkProfTool::acquire(const Event* e) {

}


void CilkProfTool::call(const Event* e) {

	// F spawns or calls G:
	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	CallInfo *_info = e->cilkprof.getCallInfo();
	ShadowThread* childThread = _info->childThread;
	const char* G_signature = this.threadFunctionMap[childThread.threadId];
	this->cilkprof->setFunctionWorkSpan(_info->fnSignature, 0.0, 0.0, 0.0, 0.0);
}


// memory access event - the only related event to returns
void CilkProfTool::access(const Event* e) {

	AccessInfo *_info = e->cilkprof.getAccessInfo();

}

// lock release event: IMPORTANT
void CilkProfTool::release(const Event* e) {

	ReleaseInfo *_info = e->cilkprof.getReleaseInfo();
}



void CilkProfTool::returnOfCalled(const Event* e){

	// Called G returns to F:
	// G.p += G.c
	// F.w += G.w
	// F.c += G.p

	cilkprof.addToPrefix(G_signature, cilkprof.getContinuation(G_signature));
	cilkprof.addToWork(F_signature, cilkprof.getWork(G_signature));
	cilkprof.addToContinuation(F_signature, cilkprof.getPrefix(G_signature));
}


	// F syncs
	// if F.c > F.l
	// 		F.p += F.c
	// else
	// 		F.p += F.l
	// F.c = 0
	// F.l = 0

	// AcquireInfo *_info = e->cilkprof.getAcquireInfo();

	const char* F_signature = this.threadFunctionMap[currentThread.threadId];

	if (cilkprof.getContinuation(F_signature) > cilkprof.getLongestChild(F_signature))
			cilkprof.addToPrefix(F_signature, cilkprof.getContinuation(F_signature));
	else
			cilkprof.addToPrefix(F_signature, cilkprof.getLongestChild(F_signature));

	setContinuation(F_signature, 0.0);
	setLongestChild(F_signature, 0.0);




