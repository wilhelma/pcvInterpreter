/*
 * ParasiteTool.cpp
 *
 *  Created on: June 17, 2016
 *      Author: knapp
 */

#include "ParasiteTool.h"
#include "Parasite.h"


// converts the information in the thread and function stacks to the parasite profile information
void ParasiteTool::getEndProfile() {

  thread_frame_t* bottom_thread_frame = main_stack->thread_stack.back();
  function_frame_t* bottom_function_frame = main_stack->function_stack.back();

  double span = bottom_function_frame->running_span +
                bottom_thread_frame->prefix_span +
                bottom_thread_frame->local_span +
                bottom_thread_frame->local_continuation;

  add_call_site_hashtables(bottom_thread_frame->prefix_table, 
                           bottom_thread_frame->continuation_table);

  call_site_hashtable_t* final_span_table = bottom_thread_frame->prefix_table;

  double work = bottom_function_frame->running_work +
                bottom_function_frame->local_work;

  call_site_hashtable_t* final_work_table = main_stack->work_table;

  // TODO: iterate over work table an span table to print information


}

// calculates the profile information, then prints out the profile 
void ParasiteTool::printProfile(){
  
  getEndProfile();

  FILE *fout;
  char filename[64];
}


ParasiteTool::ParasiteTool() {

  min_capacity = 8;
  main_stack = new thread_stack_t();
  thread_stack_init(main_stack, MAIN);
}

ParasiteTool::~ParasiteTool() {

  printProfile();
}


void ParasiteTool::create(const Event* e) {

	NewThreadEvent* newThreadEvent = (NewThreadEvent *) e;
	const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
	TRD_ID newThreadID = _info->childThread->threadId;
	TIME create_time = _info->startTime;
}

// this is a SYNC EVENT 
void ParasiteTool::join(const Event* e) {

	JoinEvent* joinEvent = (JoinEvent*) e;
	const JoinInfo *_info = joinEvent->getJoinInfo();
	TRD_ID childThreadId = _info->childThread->threadId;
	TRD_ID parentThreadId = _info->parentThread->threadId;
  TIME join_time = _info->joinTime;
}

void ParasiteTool::call(const Event* e) {

	CallEvent* callEvent = (CallEvent*) e;
	const CallInfo *_info = callEvent->getCallInfo();

	FUN_SG calledFunctionSignature = _info->fnSignature;
	TRD_ID calledThreadID = callEvent->getThread()->threadId;
	CALLSITE calledSiteID = _info->siteId;
	TIME callTime = _info->startTime;
}

void ParasiteTool::returnOfCalled(const Event* e) {


}

void ParasiteTool::threadEnd(const Event* e) {


}

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {

	// AcquireEvent* acquireEvent = (AcquireEvent*) e;
	// const AcquireInfo *_info = acquireEvent->getAcquireInfo();
	// ShadowLock *acquiredLock = _info->lock;

  // acquire_time = e->runtime
	// acquiredLock->last_acquire_time = acquire_time;
}

// lock release event
void ParasiteTool::release(const Event* e) {

	// ReleaseEvent* releaseEvent = (ReleaseEvent*) e;
	// const ReleaseInfo *_info = releaseEvent->getReleaseInfo();
	// ShadowLock *releasedLock = _info->lock;

}

// lock release event
void ParasiteTool::access(const Event* e) {

  // DOES NOTHING
  return;
}




