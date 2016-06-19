/*
 * ParasiteTool.cpp
 *
 *  Created on: June 17, 2016
 *      Author: knapp
 */

#include "ParasiteTool.h"
#include "Parasite.h"

// converts call site information collected to the end profile with the needed information
void getEndCallSiteWorkProfile(call_site_profile_t* collected_profile, call_site_end_profile_t* end_profile) {


  // work excluding recurisve calls
  end_profile->work_work = collected_profile->work;
  end_profile->span_work = collected_profile->span;
  end_profile->parallelism_work = end_profile->work_work/end_profile->span_work;
  end_profile->count_work = collected_profile->count;

  // work data from top calls of call site
  end_profile->top_work_work = collected_profile->top_work;
  end_profile->top_span_work = collected_profile->top_span;
  end_profile->top_parallelism_work = end_profile->top_work_work/end_profile->top_span_work;
  end_profile->top_count_work = collected_profile->top_count;

  // local(TODO: explain what local means) work call site data
  end_profile->local_work_work = collected_profile->local_work;
  end_profile->local_span_work = collected_profile->local_span;
  end_profile->local_parallelism_work = end_profile->top_work_work/end_profile->top_span_work;
  end_profile->local_count_work = collected_profile->top_count

};

void getEndCallSiteSpanProfile(call_site_profile_t* collected_profile, call_site_end_profile_t* end_profile) {

 // span data excluding recursive calls
  end_profile->work_span = collected_profile->work;
  end_profile->span_span = collected_profile->span;
  end_profile->parallelism_span = collected_profile->work_span / collected_profile->span_span;
  end_profile->count_span = collected_profile->count;

  // data from top calls of call site
  end_profile->top_work_span = collected_profile->top_work;
  end_profile->top_span_span = collected_profile->top_span;
  end_profile->top_parallelism_span = collected_profile->top_work_span / collected_profile->top_span_span;
  end_profile->top_count_span = collected_profile->top_count;

  // local(?) span call site data
  end_profile->local_work_span = collected_profile->local_work;
  end_profile->local_span_span = collected_profile->local_span;
  end_profile->local_parallelism_span = collected_profile->local_work_span / collected_profile->local_span_span;
  end_profile->local_count_span  = collected_profile->local_count;

};



// converts the information in the thread and function stacks to the parasite profile information
void ParasiteTool::getEndProfile() {

  thread_frame_t* bottom_thread_frame = main_stack->thread_stack.back();
  function_frame_t* bottom_function_frame = main_stack->function_stack.back();

  // Calculate span for entire program 
  parasite_profile->span = bottom_function_frame->running_span +
                bottom_thread_frame->prefix_span +
                bottom_thread_frame->local_span +
                bottom_thread_frame->local_continuation;

  add_call_site_hashtables(bottom_thread_frame->prefix_table, 
                           bottom_thread_frame->continuation_table);

  call_site_hashtable_t* final_span_table = bottom_thread_frame->prefix_table;

  // Calculate work for entire program
  parasite_profile->work = bottom_function_frame->running_work +
                           bottom_function_frame->local_work;

  // Calculate parallelism for entire program                     
  parasite_profile->parallelism = parasite_profile->work / parasite_profile->span;

  call_site_hashtable_t* final_work_table = main_stack->work_table;
  call_site_print_info_hashtable_t* call_site_print_info_hashtable_t; 

  // parse the final work table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected work profiles of call sites. 
  for (auto const &it : *final_work_table) {

    call_site_profile_t* current_call_site_profile = it.second;
    call_site_profile_t* current_call_site_ID = it.first;
    call_site_end_profile_t* current_call_site_end_profile;
    getEndCallSiteWorkProfile(current_call_site_profile, current_call_site_end_profile);

    // add work information into the final profile for each call site 
    call_site_end_profile.insert(current_call_site_ID, current_call_site_end_profile);
  }

  // parse the final span table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected span profiles of call sites. 
  for (auto const &it : *final_span_table) {

    call_site_profile_t* current_call_site_profile = it.second;
    call_site_profile_t* current_call_site_ID = it.first;
    call_site_end_profile_t* current_call_site_end_profile = end_call_site_profile_hashtable[current_call_site_ID];

    // add span information into the final profile for each call site 
    getEndCallSiteSpanProfile(current_call_site_profile, current_call_site_end_profile);
  }
}

// calculates the profile information, then prints out the profile 
void ParasiteTool::printProfile(){
  
  // TODO:
  // 1. decide which format to use for printing overall parasite profile
  // 2. decide which format to use for printing each call site's profile
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




