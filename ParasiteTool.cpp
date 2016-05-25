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
#include "ParasiteToolUtilities.h"


ParasiteTool::ParasiteTool() {

  cilkprof_stack_t *stack = &(GET_STACK(ctx_stack));

  if (!TOOL_INITIALIZED) {
    initialize_tool(&(ctx_stack));

  } else {
    stack = &(GET_STACK(ctx_stack));

    uint64_t strand_len = measure_and_add_strand_length(stack);
    if (stack->bot->c_head == stack->c_tail) {
      stack->bot->local_contin += strand_len;
    }
  }

  // Push new frame onto the stack
  cilkprof_stack_push(stack, SPAWNER);

  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(rip);
  uintptr_t fn = (uintptr_t)this_fn;

  int32_t cs_index = add_to_iaddr_table(&call_site_table, cs, SPAWNER);
  c_bottom->cs_index = cs_index;
  if (cs_index >= stack->cs_status_capacity) {
    resize_cs_status_vector(&(stack->cs_status), &(stack->cs_status_capacity));
  }
  int32_t cs_tail = stack->cs_status[cs_index].c_tail;
  if (OFF_STACK != cs_tail) {
    if (!(stack->cs_status[cs_index].flags & RECURSIVE)) {
      stack->cs_status[cs_index].flags |= RECURSIVE;
    }
  } else {
    int32_t fn_index;
    if (UNINITIALIZED == stack->cs_status[cs_index].fn_index) {

      assert(call_site_table->table_size == cs_index + 1);
      MIN_CAPACITY = cs_index + 1;

      fn_index = add_to_iaddr_table(&function_table, fn, SPAWNER);
      stack->cs_status[cs_index].fn_index = fn_index;
      if (fn_index >= stack->fn_status_capacity) {
        resize_fn_status_vector(&(stack->fn_status), &(stack->fn_status_capacity));
      }
    } else {
      fn_index = stack->cs_status[cs_index].fn_index;
    }
    stack->cs_status[cs_index].c_tail = stack->c_tail;
    if (OFF_STACK == stack->fn_status[fn_index]) {
      stack->fn_status[fn_index] = stack->c_tail;
    }
  }

}

}

ParasiteTool::~ParasiteTool() {



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

