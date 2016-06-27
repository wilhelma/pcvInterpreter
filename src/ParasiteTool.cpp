/**
 *
 *    @file  ParasiteTool.cpp
 *   @brief  Implementation file for the class `ParasiteTool`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <utility>
#include "ParasiteTool.h"

void getEndCallSiteWorkProfile(std::shared_ptr<call_site_profile_t>  collected_profile, 
                               std::shared_ptr<call_site_end_profile_t> end_profile) {
  // work excluding recurisve calls
  end_profile->work_work = collected_profile->work;
  end_profile->span_work = collected_profile->span;
  end_profile->parallelism_work = end_profile->work_work/end_profile->span_work;
  end_profile->count_work = collected_profile->count;

  // work data from top calls of call site
  end_profile->top_work_work = collected_profile->top_work;
  end_profile->top_span_work = collected_profile->top_span;
  end_profile->top_parallelism_work = end_profile->top_work_work/
                                      end_profile->top_span_work;
  end_profile->top_count_work = collected_profile->top_count;

  // local(TODO: explain what local means) work call site data
  end_profile->local_work_work = collected_profile->local_work;
  end_profile->local_span_work = collected_profile->local_span;
  end_profile->local_parallelism_work = end_profile->top_work_work/
                                        end_profile->top_span_work;
  end_profile->local_count_work = collected_profile->top_count;
}

void getEndCallSiteSpanProfile(std::shared_ptr<call_site_profile_t> collected_profile, 
                               std::shared_ptr<call_site_end_profile_t> end_profile) {
  // span data excluding recursive calls
  end_profile->work_span = collected_profile->work;
  end_profile->span_span = collected_profile->span;
  end_profile->parallelism_span = end_profile->work_span/ 
                                  end_profile->span_span;
  end_profile->count_span = collected_profile->count;

  // data from top calls of call site
  end_profile->top_work_span = collected_profile->top_work;
  end_profile->top_span_span = collected_profile->top_span;
  end_profile->top_parallelism_span = end_profile->top_work_span
                                    / end_profile->top_span_span;
  end_profile->top_count_span = collected_profile->top_count;

  // local(?) span call site data
  end_profile->local_work_span = collected_profile->local_work;
  end_profile->local_span_span = collected_profile->local_span;
  end_profile->local_parallelism_span = end_profile->local_work_span / 
                                        end_profile->local_span_span;
  end_profile->local_count_span  = collected_profile->local_count;
}

ParasiteTool::ParasiteTool() {}


// converts the information in the thread and function stacks 
// to the parasite profile information
void ParasiteTool::getEndProfile() {
  std::shared_ptr<thread_frame_t> bottom_thread_frame = 
                                  main_stack->thread_stack.back();
  std::shared_ptr<function_frame_t> bottom_function_frame = 
                                    main_stack->function_stack.back();

  // Calculate span for entire program 
  parasite_profile->span = bottom_function_frame->running_span +
                bottom_thread_frame->prefix_span +
                bottom_thread_frame->local_span +
                bottom_thread_frame->local_continuation;

  CallSiteHashtable bottom_prefix_table(bottom_thread_frame->prefix_table);

  bottom_prefix_table.add_in_hashtable(bottom_thread_frame->continuation_table);

  std::shared_ptr<call_site_hashtable_t> final_span_table =
                                             bottom_thread_frame->prefix_table;

  // Calculate work for entire program
  parasite_profile->work = bottom_function_frame->running_work +
                           bottom_function_frame->local_work;

  // Calculate parallelism for entire program                     
  parasite_profile->parallelism = parasite_profile->work 
                                / parasite_profile->span;

  std::shared_ptr<call_site_hashtable_t> final_work_table = 
                                         main_stack->work_table;

  // parse the final work table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected
  // work profiles of call sites. 
  for (auto const &it : *final_work_table) {
    std::shared_ptr<call_site_profile_t> current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    std::shared_ptr<call_site_end_profile_t> current_call_site_end_profile
                                            (new call_site_end_profile_t());
    getEndCallSiteWorkProfile(current_call_site_profile, 
                              current_call_site_end_profile);

    // add work information into the final profile for each call site 
    end_call_site_profile_hashtable->at(current_call_site_ID) = 
                                        current_call_site_end_profile;
  }

  // parse the final span table in the main stack data structure 
  // iterate through all entries in the hashtable containing 
  // collected span profiles of call sites. 
  for (auto const &it : *final_span_table) {
    std::shared_ptr<call_site_profile_t> current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    std::shared_ptr<call_site_end_profile_t> current_call_site_end_profile = 
    end_call_site_profile_hashtable->at(current_call_site_ID);

    // add span information into the final profile for each call site 
    getEndCallSiteSpanProfile(current_call_site_profile, 
                              current_call_site_end_profile);
  }
}

// calculates the profile information, then prints out the profile 
void ParasiteTool::printProfile() {
  // first, calculate all the end profiles before outputting them 
  this->getEndProfile();
  
  // TODO(knapp):
  // 1. decide which format to use for printing overall parasite profile
  // 2. decide which format to use for printing each call site's profile
}

ParasiteTool::~ParasiteTool() {
  printProfile();
}


// actions for creating a thread
void ParasiteTool::create(const Event* e) {
  NewThreadEvent* newThreadEvnt = (NewThreadEvent*) e;
	std::shared_ptr<NewThreadEvent> newThreadEvent(newThreadEvnt);
  const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
  TRD_ID newThreadID = _info->childThread->threadId;
  TIME create_time = _info->startTime;

  double strand_length = create_time - last_strand_start_time;
  std::shared_ptr<thread_frame_t> bottom_thread_frame(main_stack->
                                                      thread_stack.back());
  bottom_thread_frame->local_continuation += strand_length;
  main_stack->thread_stack_push();
  std::shared_ptr<thread_frame_t> new_thread_frame = 
                  main_stack->thread_stack.at(main_stack->current_thread_index);

  new_thread_frame->thread = newThreadID;

  main_stack->init_thread_frame(main_stack->current_thread_index + 1, 
                                main_stack->current_function_index + 1);
  main_stack->current_thread_index += 1;
}

void ParasiteTool::join(const Event* e) {
  JoinEvent* joinEvnt = (JoinEvent*) e;
	std::shared_ptr<JoinEvent> joinEvent(joinEvnt);
  const JoinInfo* _info(joinEvent->getJoinInfo());

	TRD_ID childThreadId = _info->childThread->threadId;
	TRD_ID parentThreadId = _info->parentThread->threadId;

  std::shared_ptr<function_frame_t> bottom_function_frame(main_stack->
                                                        function_stack.back());
  std::shared_ptr<thread_frame_t> bottom_thread_frame(main_stack->
                                                      thread_stack.back());
  bottom_function_frame->running_span += bottom_thread_frame->
                                         local_continuation;

  if (bottom_thread_frame->longest_child_span > bottom_function_frame->
                                                running_span) {
    bottom_thread_frame->prefix_span += bottom_thread_frame->longest_child_span;
    bottom_thread_frame->prefix_span += bottom_thread_frame->lock_span;
    bottom_thread_frame->prefix_span -= bottom_thread_frame->
                                        longest_child_lock_span;
    CallSiteHashtable prefix_table(bottom_thread_frame->prefix_table);
    prefix_table.add_in_hashtable(bottom_thread_frame->longest_child_table);
    // local_span does not increase, because critical path goes 
    // through spawned child.
  } else {
    bottom_thread_frame->prefix_span += bottom_function_frame->running_span;
    // Critical path goes through continuation, which is local. Add
    // local_continuation to local_span.
    bottom_thread_frame->local_span += bottom_thread_frame->local_continuation;
    CallSiteHashtable prefix_table(bottom_thread_frame->prefix_table);
    prefix_table.add_in_hashtable(bottom_thread_frame->continuation_table);
  }

  // reset longest child and continuation span variables
  bottom_thread_frame->longest_child_span = 0;
  bottom_thread_frame->longest_child_lock_span = 0;
  bottom_function_frame->running_span = 0;
  bottom_thread_frame->local_continuation = 0;
}

// actions for calling a function
void ParasiteTool::call(const Event* e) {
  CallEvent* callEvnt = (CallEvent*) e;
	std::shared_ptr<CallEvent> callEvent(callEvnt);
	const CallInfo* _info(callEvent->getCallInfo());

	FUN_SG calledFunctionSignature = _info->fnSignature;
	TRD_ID calledThreadID = callEvent->getThread()->threadId;
	CALLSITE callsiteID = _info->siteId;
	TIME callTime = _info->startTime;

  main_stack->function_stack_push();
  std::shared_ptr<function_frame_t> new_function_frame = 
              main_stack->function_stack.at(main_stack->current_function_index);
  main_stack->init_function_frame(main_stack->current_function_index);
  main_stack->current_function_index += 1;
  new_function_frame->function_signature = calledFunctionSignature;
  new_function_frame->call_site = callsiteID;
}

// actions for returning from a function
void ParasiteTool::returnOfCalled(const Event* e) {
  ReturnEvent* returnEvnt = (ReturnEvent*) e;
  std::shared_ptr<ReturnEvent> returnEvent(returnEvnt);
  const ReturnInfo* _info(returnEvent->getReturnInfo());

  TIME returnTime = _info->endTime;
  double local_work = returnTime - last_strand_start_time;
  last_strand_start_time = returnTime;

  std::shared_ptr<function_frame_t> returned_function_frame(main_stack->
                                                         function_stack.back());
  CALLSITE returning_call_site = returned_function_frame->call_site;
  
  returned_function_frame->local_work = local_work;
  double running_work = returned_function_frame->running_work + local_work;
  double running_span = returned_function_frame->running_span + local_work;
  bool is_top_returning_function = returned_function_frame->
                                   is_top_call_site_function;

  std::shared_ptr<function_frame_t> new_bottom_function_frame = 
            main_stack->function_stack[main_stack->current_function_index - 1];
  new_bottom_function_frame->running_work += running_work;
  new_bottom_function_frame->running_span += running_span;
  new_bottom_function_frame->running_lock_span += returned_function_frame->
                                                  running_lock_span;

  CallSiteHashtable work_table(main_stack->work_table);
  CallSiteHashtable bottom_thread_continuation_table(main_stack->
                                                    thread_stack.back()->
                                                    continuation_table);

  if (is_top_returning_function) {
    work_table.add_data_to_hashtable(is_top_returning_function,
                              returning_call_site, 
                              running_work, running_span,
                              local_work, local_work);

    bottom_thread_continuation_table.add_data_to_hashtable(
                              is_top_returning_function,
                              returning_call_site, 
                              running_work, running_span,
                              local_work, local_work);
  } else {
    work_table.add_local_data_to_hashtable(returning_call_site, 
                              local_work, local_work);

    bottom_thread_continuation_table.add_local_data_to_hashtable(
      returning_call_site, 
                              local_work, local_work);
  }

  // pop_back() destroys the returned funciton frame, so popping function 
  // should be the last step here
  main_stack->function_stack.pop_back();
  main_stack->current_function_index -= 1;
}

void ParasiteTool::threadEnd(const Event* e) {
  //ThreadEndEvent* threadEndEvnt = reinterpret_cast<ThreadEndEvent*> (e);
  // std::shared_ptr<ThreadEndEvent> threadEndEvent(threadEvnt);
  // const std::shared_ptr<ThreadEndInfo> _info(threadEndEvent->
  //                                            getThreadEndInfo());
  // TIME threadEndTime = _info->threadEndTime;
  TIME threadEndTime = (TIME) 0;
  double strand_length = threadEndTime - last_strand_start_time;
  std::shared_ptr<thread_frame_t> ending_thread_frame(main_stack->
                                                      thread_stack.back());
  std::shared_ptr<thread_frame_t> 
       parent_thread_frame(main_stack->thread_stack.
                                      at(main_stack->current_thread_index - 1));

  ending_thread_frame->local_continuation += strand_length;
  std::shared_ptr<function_frame_t> old_bottom_function_frame(
                                            main_stack->function_stack.back());
  std::shared_ptr<function_frame_t> 
             new_bottom_function_frame(main_stack->function_stack
                                      [main_stack->current_function_index - 1]);

  ending_thread_frame->prefix_span += old_bottom_function_frame->running_span;
  ending_thread_frame->local_span += ending_thread_frame->local_continuation;
  old_bottom_function_frame->running_work += old_bottom_function_frame->
                                             local_work;
  ending_thread_frame->prefix_span += ending_thread_frame->local_span;

  // CHECK if operations on function frame are necessary for threadEvent
  new_bottom_function_frame->running_work += old_bottom_function_frame->
                                             running_work;
  bool is_top_call_site_function = old_bottom_function_frame->
                                   is_top_call_site_function;

  CallSiteHashtable work_table(main_stack->work_table);
  CallSiteHashtable bottom_thread_prefix_table(main_stack->thread_stack.back()
                                                          ->prefix_table);
  if (is_top_call_site_function) {
    work_table.add_data_to_hashtable(old_bottom_function_frame->
                                     is_top_call_site_function,
                              old_bottom_function_frame->call_site, 
                              old_bottom_function_frame->running_work, 
                              ending_thread_frame->prefix_span,
                              old_bottom_function_frame->local_work, 
                              ending_thread_frame->local_span);

    bottom_thread_prefix_table.add_data_to_hashtable(
                          old_bottom_function_frame->is_top_call_site_function,
                          old_bottom_function_frame->call_site, 
                          old_bottom_function_frame->running_work, 
                          ending_thread_frame->prefix_span,
                          old_bottom_function_frame->local_work, 
                          ending_thread_frame->local_span);
  } else {
    work_table.add_local_data_to_hashtable(old_bottom_function_frame->call_site, 
                              old_bottom_function_frame->local_work, 
                              ending_thread_frame->local_span);

    bottom_thread_prefix_table.add_local_data_to_hashtable(
                              old_bottom_function_frame->call_site, 
                              old_bottom_function_frame->local_work, 
                              ending_thread_frame->local_span);
  }

  if (new_bottom_function_frame->running_span 
      + ending_thread_frame->local_continuation
                                   > ending_thread_frame->longest_child_span) {
      parent_thread_frame->prefix_span += new_bottom_function_frame->
                                          running_span;
      parent_thread_frame->local_span += parent_thread_frame->
                                        local_continuation;

      CallSiteHashtable prefix_table(parent_thread_frame->prefix_table);
      prefix_table.add_in_hashtable(parent_thread_frame->continuation_table);

      // Save old bottom thread frame tables in 
      // parent frame's longest child variable.
      parent_thread_frame->longest_child_span = ending_thread_frame->
                                                prefix_span;
      parent_thread_frame->longest_child_table->clear();

      std::shared_ptr<call_site_hashtable_t> temp_hashtable(parent_thread_frame
                                                        ->longest_child_table);
      parent_thread_frame->longest_child_table = 
                                              ending_thread_frame->prefix_table;
      ending_thread_frame->prefix_table = temp_hashtable;

      ending_thread_frame->longest_child_table->clear();
      ending_thread_frame->continuation_table->clear();
  } else {
      ending_thread_frame->prefix_table->clear();
      ending_thread_frame->longest_child_table->clear();
      ending_thread_frame->continuation_table->clear();
  }

  // pop the thread off the stack last, 
  // because the pop operation destroys the frame
  main_stack->thread_stack.pop_back();
  main_stack->current_thread_index -= 1;
  }

// lock acquire event 
void ParasiteTool::acquire(const Event* e) {
  // AcquireEvent* acquireEvnt = reinterpret_cast<AcquireEvent*>(e);
	// std::shared_ptr<AcquireEvent> acquireEvent(e);
	// const AcquireInfo *_info = acquireEvent->getAcquireInfo();
	// ShadowLock *acquiredLock = _info->lock;
	// acquiredLock->last_acquire_time = e->acquireTime;

  // DUMMY LINES - will delete when AcquireEvent is implemented

  // unsigned int lockId = acquiredLock->lockId;
  unsigned int lockId = (unsigned int) 0;

  if (lock_hashtable.count(lockId)) {
    lock_hashtable.at(lockId) = main_stack->current_function_index;
  } else {
    std::pair<unsigned int, int> newPair(lockId, 
                                         main_stack->current_function_index);
    lock_hashtable.insert(newPair);
  }

  total_locks_running += 1;
}

// lock release event
void ParasiteTool::release(const Event* e) {
  // ReleaseEvent* releaseEvnt = reinterpret_cast<ReleaseEvent*>(e);
	// std::shared_ptr<ReleaseEvent> releaseEvent(e);
	// const std::shared_ptr<ReleaseInfo> _info(releaseEvent->getReleaseInfo());
	// std::shared_ptr<ShadowLock> releasedLock(_info->lock);

  // release_time = e->releaseTime;

  // DUMMY lines used for debugging - 
  // will delete when ReleaseEvent is implemented:

  // double lock_span = release_time - releasedLock->last_acquire_time;
  double lock_span = 0.0;

  // unsigned int lockId = releasedLock->lockId;
  unsigned int lockId = (unsigned int) 0;

  int unlocked_function_index = lock_hashtable.at(lockId);
  std::shared_ptr<function_frame_t> unlocked_function_frame(
                        main_stack->function_stack.at(unlocked_function_index));

  if (total_locks_running == 1)
    unlocked_function_frame->local_lock_span += lock_span;

  total_locks_running -= 1;
}

// lock access event
void ParasiteTool::access(const Event* e) {
  printf("ERROR: Parasite Tool does not implement access event");
  return;
}




