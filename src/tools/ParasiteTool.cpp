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


/**
*    @fn getEndCallSiteWorkProfile(std::shared_ptr<call_site_profile_t>  collected_profile, 
                                   std::shared_ptr<call_site_end_profile_t> end_profile)
*    @brief Converts work and span information in collected_profile to more detailed work 
            information contained in end profile. 
*/
            
void getEndCallSiteWorkProfile(std::shared_ptr<call_site_profile_t>  collected_profile, 
                               std::shared_ptr<call_site_end_profile_t> end_profile) {
  // work excluding recurisve calls
  end_profile->work = collected_profile->work;
  end_profile->span = collected_profile->span;
  end_profile->parallelism = end_profile->work/end_profile->span;
  end_profile->count = collected_profile->count;

  // work data from top calls of call site
  end_profile->top_work = collected_profile->top_work;
  end_profile->top_span = collected_profile->top_span;
  end_profile->top_parallelism = end_profile->top_work/end_profile->top_span;
  end_profile->top_count = collected_profile->top_count;

  // local(TODO: explain what local means) work call site data
  end_profile->local_work = collected_profile->local_work;
  end_profile->local_span = collected_profile->local_span;
  end_profile->local_parallelism = end_profile->top_work/end_profile->top_span;
  end_profile->local_count = collected_profile->top_count;
}

/**
*    @fn getEndCallSiteSpanProfile(std::shared_ptr<call_site_profile_t>  collected_profile, 
                                   std::shared_ptr<call_site_end_profile_t> end_profile)
*    @brief Converts on_span information in collected_profile to more detailed 
            on_span information contained in end_profile. The on_span information is work 
            and span data from invocations on the critical path. 
*/
void getEndCallSiteSpanProfile(std::shared_ptr<call_site_profile_t> collected_profile, 
                               std::shared_ptr<call_site_end_profile_t> end_profile) {
  // span data excluding recursive calls
  end_profile->work_on_span = collected_profile->work;
  end_profile->span_on_span = collected_profile->span;
  end_profile->parallelism_on_span = end_profile->work_on_span/ 
                                     end_profile->span_on_span;
  end_profile->count_on_span = collected_profile->count;

  // data from top calls of call site
  end_profile->top_work_on_span = collected_profile->top_work;
  end_profile->top_span_on_span = collected_profile->top_span;
  end_profile->top_parallelism_on_span = end_profile->top_work_on_span
                                    / end_profile->top_span_on_span;
  end_profile->top_count_on_span = collected_profile->top_count;

  // local(?) span call site data
  end_profile->local_work_on_span = collected_profile->local_work;
  end_profile->local_span_on_span = collected_profile->local_span;
  end_profile->local_parallelism_on_span = end_profile->local_work_on_span / 
                                           end_profile->local_span_on_span;
  end_profile->local_count_on_span = collected_profile->local_count;
}

ParasiteTool::ParasiteTool() {

  stacks = std::unique_ptr<ParasiteTracker>(new ParasiteTracker);
  parasite_profile = std::unique_ptr<parasite_profile_t>(new parasite_profile_t);
  end_call_site_profile_hashtable = std::unique_ptr<call_site_end_hashtable_t>(new call_site_end_hashtable_t);

  std::unordered_map<unsigned int, int> lck_hashtable;
  lock_hashtable = lck_hashtable;

  last_strand_start_time = (TIME) 0.0;
  last_function_runtime = (TIME) 0.0;
  total_locks_running = 0;
}

void ParasiteTool::getEndProfile() {

  std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();
  std::shared_ptr<function_frame_t> bottom_function_frame = stacks->bottomFunction();

  // Calculate span for entire program 
  parasite_profile->span =  bottom_function_frame->running_span +
                            bottom_thread_frame->prefix_span +
                            bottom_thread_frame->local_span +
                            bottom_thread_frame->local_continuation;

  CallSiteHashtable bottom_prefix_table(bottom_thread_frame->prefix_table);

  bottom_prefix_table.add_in_hashtable(bottom_thread_frame->continuation_table);

  std::shared_ptr<call_site_hashtable_t> final_on_span_table = bottom_thread_frame->prefix_table;

  // Calculate work for entire program
  parasite_profile->work = bottom_function_frame->running_work +
                           bottom_function_frame->local_work;

  // Calculate parallelism for entire program                     
  parasite_profile->parallelism = parasite_profile->work 
                                / parasite_profile->span;

  std::shared_ptr<call_site_hashtable_t> final_table = stacks->work_table;

  // parse the final work table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected
  // work profiles of call sites. 
  for (auto const &it : *final_table) {
    std::shared_ptr<call_site_profile_t> current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    std::shared_ptr<call_site_end_profile_t> 
                  current_call_site_end_profile(new call_site_end_profile_t());
    getEndCallSiteWorkProfile(current_call_site_profile, 
                              current_call_site_end_profile);

    // add work information into the final profile for each call site 
    std::pair<CALLSITE, std::shared_ptr<call_site_end_profile_t> > 
                  newPair(current_call_site_ID, current_call_site_end_profile);
    end_call_site_profile_hashtable->insert(newPair);
  }

  // parse the final span table in the main stack data structure 
  // iterate through all entries in the hashtable containing 
  // collected span profiles of call sites. 
  for (auto const &it : *final_on_span_table) {
    std::shared_ptr<call_site_profile_t> current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    std::shared_ptr<call_site_end_profile_t> current_call_site_end_profile = 
    end_call_site_profile_hashtable->at(current_call_site_ID);

    // add span information into the final profile for each call site 
    getEndCallSiteSpanProfile(current_call_site_profile, 
                              current_call_site_end_profile);
  }
}

void ParasiteTool::printProfile() {

  // first, calculate all the end profiles before outputting them 
  this->getEndProfile();

  printf("PARALLELISM IS %f \n", parasite_profile->parallelism);
  printf("WORK IS %f \n", parasite_profile->work);
  printf("SPAN IS %f \n", parasite_profile->span);
  printf("LOCK SPAN IS %f \n", parasite_profile->lock_span);

}

ParasiteTool::~ParasiteTool() {
  printProfile();
}

void ParasiteTool::create(const Event* e) {

  printf("starting new thread Event \n");

  NewThreadEvent* newThreadEvent = (NewThreadEvent*) e;
  const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
  const TRD_ID newThreadID(_info->childThread->threadId);
  TIME create_time = _info->startTime;
  double strand_length = create_time - last_strand_start_time;

  stacks->thread_stack_push();
  std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomParentThread();
  bottom_thread_frame->local_continuation += strand_length;
  std::shared_ptr<thread_frame_t> new_thread_frame = stacks->bottomThread();
  new_thread_frame->thread = newThreadID;

  stacks->init_thread_frame(stacks->current_thread_index, 
                                stacks->current_function_index + 1);

  // get information about the thread's head function
  FUN_SG calledFunctionSignature = _info->childThread->currentFunctionSignature;
  CALLSITE callsiteID = _info->childThread->currentCallSiteID;
  last_strand_start_time = _info->startTime;

  // push the thread's function onto the stack
  stacks->function_stack_push();
  std::shared_ptr<function_frame_t> new_function_frame = stacks->bottomFunction();
  stacks->init_function_frame(stacks->current_function_index);
  new_function_frame->function_signature = calledFunctionSignature;
  new_function_frame->call_site = callsiteID;
  new_function_frame->is_top_call_site_function = true;

  printf("ending new thread Event \n");
}

void ParasiteTool::join(const Event* e) {

  printf("starting join Event");
  JoinEvent* joinEvent = (JoinEvent*) e;
  const JoinInfo* _info(joinEvent->getJoinInfo());

	TRD_ID childThreadId = _info->childThread->threadId;
	TRD_ID parentThreadId = _info->parentThread->threadId;

  std::shared_ptr<function_frame_t> bottom_function_frame(stacks->bottomFunction());
  std::shared_ptr<thread_frame_t> bottom_thread_frame(stacks->bottomThread());
  bottom_function_frame->running_span += bottom_thread_frame->local_continuation;

  if (bottom_thread_frame->longest_child_span > bottom_function_frame-> running_span) {
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

  printf("ending join Event \n");
}

void ParasiteTool::call(const Event* e) {

  printf("starting call Event \n");
  CallEvent* callEvent = (CallEvent*) e;
	const CallInfo* _info(callEvent->getCallInfo());

	FUN_SG calledFunctionSignature = _info->fnSignature;
	TRD_ID calledThreadID = callEvent->getThread()->threadId;
	CALLSITE callsiteID = _info->siteId;
  last_function_runtime = _info->runtime;

  stacks->function_stack_push();
  std::shared_ptr<function_frame_t> new_function_frame = stacks->bottomFunction();
  stacks->init_function_frame(stacks->current_function_index);
  new_function_frame->function_signature = calledFunctionSignature;
  new_function_frame->call_site = callsiteID;
  new_function_frame->is_top_call_site_function = false;

  printf("ending call Event \n");
}

void ParasiteTool::returnOperations(double local_work) {

  std::shared_ptr<function_frame_t> returned_function_frame(stacks->bottomFunction());
  CALLSITE returning_call_site = returned_function_frame->call_site;
  
  returned_function_frame->local_work = local_work;
  double running_work = returned_function_frame->running_work + local_work;
  double running_span = returned_function_frame->running_span + local_work;
  bool is_top_returning_function = returned_function_frame->is_top_call_site_function;

  std::shared_ptr<function_frame_t> current_function_frame = stacks->parentFunction();
  current_function_frame->running_work += running_work;
  current_function_frame->running_span += running_span;
  current_function_frame->running_lock_span += returned_function_frame->
                                                  running_lock_span;

  CallSiteHashtable work_table(stacks->work_table);
  CallSiteHashtable bottom_thread_continuation_table(stacks->
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

    work_table.add_local_data_to_hashtable(returning_call_site, local_work, local_work);
    bottom_thread_continuation_table.add_local_data_to_hashtable(returning_call_site, local_work, local_work);
  }

  stacks->function_stack_pop();
}

void ParasiteTool::returnOfCalled(const Event* e) {

  printf("starting return Event \n");

  ReturnEvent* returnEvent = (ReturnEvent*) e;
  const ReturnInfo* _info(returnEvent->getReturnInfo());

  TIME returnTime = _info->endTime;
  double local_work = last_function_runtime;
  // double local_work = returnTime - last_strand_start_time;
  last_strand_start_time = returnTime;

  returnOperations(local_work);
  printf("ending return Event \n");
}

void ParasiteTool::threadEnd(const Event* e) {

  printf("starting thread end Event \n");

  ThreadEndEvent* threadEndEvent = (ThreadEndEvent*) e;
  const ThreadEndInfo* _info(threadEndEvent->getThreadEndInfo());
  TIME threadEndTime = _info->endTime;


  double local_work = threadEndTime - last_strand_start_time;

  returnOperations(local_work);

  if (stacks->current_thread_index == 0) {

    printf("starting thread end Event operations for main thread \n");

    std::shared_ptr<thread_frame_t> ending_thread_frame(stacks->bottomThread());
    ending_thread_frame->local_continuation += local_work;
    std::shared_ptr<function_frame_t> parent_function_frame(stacks->bottomFunction());

    ending_thread_frame->prefix_span += parent_function_frame->running_span;
    ending_thread_frame->local_span += ending_thread_frame->local_continuation;
    parent_function_frame->running_work += parent_function_frame->local_work;
    ending_thread_frame->prefix_span += ending_thread_frame->local_span;

    CallSiteHashtable work_table(stacks->work_table);
    CallSiteHashtable bottom_thread_prefix_table(stacks->bottomThread()->prefix_table);

    work_table.add_data_to_hashtable(parent_function_frame->
                                    is_top_call_site_function,
                                    parent_function_frame->call_site, 
                                    parent_function_frame->running_work, 
                                    ending_thread_frame->prefix_span,
                                    parent_function_frame->local_work, 
                                    ending_thread_frame->local_span);

    bottom_thread_prefix_table.add_data_to_hashtable(
                          parent_function_frame->is_top_call_site_function,
                          parent_function_frame->call_site, 
                          parent_function_frame->running_work, 
                          ending_thread_frame->prefix_span,
                          parent_function_frame->local_work, 
                          ending_thread_frame->local_span);
    
    printf("ENDING MAIN THREAD \n");
    return;
  }


  std::shared_ptr<thread_frame_t> parent_thread_frame(stacks->bottomParentThread());
  std::shared_ptr<thread_frame_t> ending_thread_frame(stacks->bottomThread());

  ending_thread_frame->local_continuation += local_work;

  std::shared_ptr<function_frame_t> parent_function_frame(stacks->bottomParentFunction());
  std::shared_ptr<function_frame_t> current_function_frame(stacks->bottomFunction());

  ending_thread_frame->prefix_span += parent_function_frame->running_span;
  ending_thread_frame->local_span += ending_thread_frame->local_continuation;
  parent_function_frame->running_work += parent_function_frame->local_work;
  ending_thread_frame->prefix_span += ending_thread_frame->local_span;

  current_function_frame->running_work += parent_function_frame->running_work;

  bool is_top_call_site_function = parent_function_frame->is_top_call_site_function;

  CallSiteHashtable work_table(stacks->work_table);
  CallSiteHashtable bottom_thread_prefix_table(stacks->bottomThread()->prefix_table);

  if (is_top_call_site_function) {

    work_table.add_data_to_hashtable(parent_function_frame->
                                      is_top_call_site_function,
                                      parent_function_frame->call_site, 
                                      parent_function_frame->running_work, 
                                      ending_thread_frame->prefix_span,
                                      parent_function_frame->local_work, 
                                      ending_thread_frame->local_span);

    bottom_thread_prefix_table.add_data_to_hashtable(
                          parent_function_frame->is_top_call_site_function,
                          parent_function_frame->call_site, 
                          parent_function_frame->running_work, 
                          ending_thread_frame->prefix_span,
                          parent_function_frame->local_work, 
                          ending_thread_frame->local_span);
  } else {


    work_table.add_local_data_to_hashtable(parent_function_frame->call_site, 
                              parent_function_frame->local_work, 
                              ending_thread_frame->local_span);

    bottom_thread_prefix_table.add_local_data_to_hashtable(
                              parent_function_frame->call_site, 
                              parent_function_frame->local_work, 
                              ending_thread_frame->local_span);
  }

  if (current_function_frame->running_span + ending_thread_frame->local_continuation  
                                              > ending_thread_frame->longest_child_span) {

    parent_thread_frame->prefix_span += current_function_frame->running_span;
    parent_thread_frame->local_span += parent_thread_frame->local_continuation;

    CallSiteHashtable prefix_table(parent_thread_frame->prefix_table);
    prefix_table.add_in_hashtable(parent_thread_frame->continuation_table);

    // Save old bottom thread frame tables in 
    // parent frame's longest child variable.
    parent_thread_frame->longest_child_span = ending_thread_frame->
                                              prefix_span;
    parent_thread_frame->longest_child_table->clear();

    std::shared_ptr<call_site_hashtable_t> temp_hashtable(parent_thread_frame->longest_child_table);
    parent_thread_frame->longest_child_table = ending_thread_frame->prefix_table;
    ending_thread_frame->prefix_table = temp_hashtable;

    ending_thread_frame->longest_child_table->clear();
    ending_thread_frame->continuation_table->clear();
  }

  else {

      ending_thread_frame->prefix_table->clear();
      ending_thread_frame->longest_child_table->clear();
      ending_thread_frame->continuation_table->clear();
  }

  // pop the thread off the stack last, 
  // because the pop operation destroys the frame
  stacks->thread_stack_pop();
  printf("ending non-main thread end Event \n");
}

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
    lock_hashtable.at(lockId) = stacks->bottomFunctionIndex();
  } else {
    std::pair<unsigned int, int> newPair(lockId, stacks->current_function_index);
    lock_hashtable.insert(newPair);
  }

  total_locks_running += 1;
}

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
  std::shared_ptr<function_frame_t> unlocked_function_frame(stacks->functionAt(unlocked_function_index));

  if (total_locks_running == 1)
    unlocked_function_frame->local_lock_span += lock_span;

  total_locks_running -= 1;
}

void ParasiteTool::access(const Event* e) {
  printf("ERROR: Parasite Tool does not implement access event");
  return;
}




