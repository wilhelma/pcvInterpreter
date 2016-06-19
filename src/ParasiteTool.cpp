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
  end_profile->local_count_work = collected_profile->top_count;

};

void getEndCallSiteSpanProfile(call_site_profile_t* collected_profile, call_site_end_profile_t* end_profile) {

 // span data excluding recursive calls
  end_profile->work_span = collected_profile->work;
  end_profile->span_span = collected_profile->span;
  end_profile->parallelism_span = end_profile->work_span / end_profile->span_span;
  end_profile->count_span = collected_profile->count;

  // data from top calls of call site
  end_profile->top_work_span = collected_profile->top_work;
  end_profile->top_span_span = collected_profile->top_span;
  end_profile->top_parallelism_span = end_profile->top_work_span / end_profile->top_span_span;
  end_profile->top_count_span = collected_profile->top_count;

  // local(?) span call site data
  end_profile->local_work_span = collected_profile->local_work;
  end_profile->local_span_span = collected_profile->local_span;
  end_profile->local_parallelism_span = end_profile->local_work_span / end_profile->local_span_span;
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

  // parse the final work table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected work profiles of call sites. 
  for (auto const &it : *final_work_table) {

    call_site_profile_t* current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    call_site_end_profile_t* current_call_site_end_profile = new call_site_end_profile_t();
    getEndCallSiteWorkProfile(current_call_site_profile, current_call_site_end_profile);

    // add work information into the final profile for each call site 
    end_call_site_profile_hashtable->at(current_call_site_ID) = current_call_site_end_profile;
  }

  // parse the final span table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected span profiles of call sites. 
  for (auto const &it : *final_span_table) {

    call_site_profile_t* current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    call_site_end_profile_t* current_call_site_end_profile = end_call_site_profile_hashtable->at(current_call_site_ID);

    // add span information into the final profile for each call site 
    getEndCallSiteSpanProfile(current_call_site_profile, current_call_site_end_profile);
  }
}

// calculates the profile information, then prints out the profile 
void ParasiteTool::printProfile(){

  // first, calculate all the end profiles before outputting them 
  this->getEndProfile();
  
  // TODO:
  // 1. decide which format to use for printing overall parasite profile
  // 2. decide which format to use for printing each call site's profile
}


ParasiteTool::ParasiteTool() {

  main_stack = new main_stack_t();
}

ParasiteTool::~ParasiteTool() {

  printProfile();
}


// IN PROGRESS:
// actions for creating a thread
// cilk spawn prepare, cilk spawn, cilk_enter_begin, 
void ParasiteTool::create(const Event* e) {

	NewThreadEvent* newThreadEvent = (NewThreadEvent *) e;
	const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
	TRD_ID newThreadID = _info->childThread->threadId;
	TIME create_time = _info->startTime;

  double strand_length = create_time - last_strand_start_time;
  thread_frame_t* bottom_thread_frame = main_stack->thread_stack.back();
  bottom_thread_frame->local_continuation += strand_length;
  thread_frame_t* new_thread_frame = thread_stack_push(main_stack);

  new_thread_frame->local_span = 0.0;
  new_thread_frame->prefix_span = 0.0;
  new_thread_frame->longest_child_span = 0.0;

  new_thread_frame->thread = newThreadID;
  new_thread_frame->head_function_index = main_stack->current_function_index + 1;
  new_thread_frame->parent_thread = main_stack->thread_stack.at(main_stack->current_thread_index);

  main_stack->current_thread_index += 1;
}

// this is a SYNC EVENT 
// cilk sync begin, cilk sync begin
void ParasiteTool::join(const Event* e) {

	JoinEvent* joinEvent = (JoinEvent*) e;
	const JoinInfo *_info = joinEvent->getJoinInfo();
	TRD_ID childThreadId = _info->childThread->threadId;
	TRD_ID parentThreadId = _info->parentThread->threadId;
  TIME join_time = (TIME) 0;//_info->joinTime;

  function_frame_t* bottom_function_frame = main_stack->function_stack.back();
  thread_frame_t* bottom_thread_frame = main_stack->thread_stack.back();
  bottom_function_frame->running_span += bottom_thread_frame->local_continuation;

  if (bottom_thread_frame->longest_child_span > bottom_function_frame->running_span) {

    bottom_thread_frame->prefix_span += bottom_thread_frame->longest_child_span;
    add_call_site_hashtables(bottom_thread_frame->prefix_table, bottom_thread_frame->longest_child_table);
    // local_span does not increase, because critical path goes through spawned child.
  }

  else {

    bottom_thread_frame->prefix_span += bottom_function_frame->running_span;
    // Critical path goes through continuation, which is local. Add
    // local_continuation to local_span.
    bottom_thread_frame->local_span += bottom_thread_frame->local_continuation;
    add_call_site_hashtables(bottom_thread_frame->prefix_table, bottom_thread_frame->continuation_table);
  }

  // reset longest child and continuation span variables
  bottom_thread_frame->longest_child_span = 0;
  bottom_function_frame->running_span = 0;
  bottom_thread_frame->local_continuation = 0;

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




