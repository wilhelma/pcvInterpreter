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

// Helper functions

ParasiteTool::ParasiteTool() {

	parasite_stack_init(main_stack, MAIN);
	call_site_table = iaddr_table_create();
	function_table = iaddr_table_create();
	begin_strand(&(main_stack));
}


ParasiteTool::~ParasiteTool() {

    cilk_tool_print(main_stack);
    parasite_stack_frame_t *old_bottom = stack->bot;
    free_cc_hashtable(stack->work_table);

    old_bottom->parent = stack->sf_free_list;
    stack->sf_free_list = old_bottom;

    parasite_stack_frame_t *free_frame = stack->sf_free_list;
    parasite_stack_frame_t *next_free_frame;

    while (NULL != free_frame) {

      next_free_frame = free_frame->parent;
      free_cc_hashtable(free_frame->prefix_table);
      free_cc_hashtable(free_frame->longest_child_table);
      free_cc_hashtable(free_frame->continuation_table);
      free(free_frame);
      free_frame = next_free_frame;
    }

    stack->sf_free_list = NULL;

    free(stack->call_site_status);
    free(stack->function_status);
    free(stack->function_stack);

    cc_hashtable_list_el_t *free_list_el = ll_free_list;
    cc_hashtable_list_el_t *next_free_list_el;
    while (NULL != free_list_el) {
      next_free_list_el = free_list_el->next;
      free(free_list_el);
      free_list_el = next_free_list_el;
    }
    ll_free_list = NULL;

    // Free the tables of call sites and functions
    iaddr_table_free(call_site_table);
    call_site_table = NULL;
    iaddr_table_free(function_table);
    function_table = NULL;
}


void ParasiteTool::create(const Event* e) {

	// F spawns  G:

	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	NewThreadEvent* newThreadEvent = (NewThreadEvent *) e;
	const NewThreadInfo *_info = newThreadEvent->getNewThreadInfo();
	const FUN_SG parentSignature = getSignature(currentThread->threadId);
	currentThread = _info->childThread;

	uint64_t strand_len = measure_and_add_strand_length(main_stack);
	stack->bot->local_continuation += strand_len;
	assert(main_stack->c_tail == main_stack->bot->c_head);
	assert(stack->c_tail == stack->bot->c_head);
	begin_strand(stack);
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


	uint64_t strand_len = measure_and_add_strand_length(stack);
	stack->bot->local_contin += strand_len;

	assert(stack->bot->c_head == stack->c_tail);

	cilkprof_stack_t *stack = &(GET_STACK(ctx_stack));

	assert(stack->bot->c_head == stack->c_tail);

	c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

	c_bottom->running_spn += stack->bot->local_contin;

	if (stack->bot->lchild_spn > c_bottom->running_spn) {
	stack->bot->prefix_spn += stack->bot->lchild_spn;

	// local_spn does not increase, because critical path goes through
	// spawned child.
	add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->lchild_table));

	} else {
	/* stack->bot->prefix_spn += stack->bot->contin_spn; */
	stack->bot->prefix_spn += c_bottom->running_spn;
	// critical path goes through continuation, which is local.  add
	// local_contin to local_spn.
	stack->bot->local_spn += stack->bot->local_contin;
	add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->contin_table));

	}

	// reset lchild and contin span variables
	stack->bot->lchild_spn = 0;
	c_bottom->running_spn = 0;
	stack->bot->local_contin = 0;
	clear_cc_hashtable(stack->bot->lchild_table);
	clear_cc_hashtable(stack->bot->contin_table);

	// can't be anything else; only SPAWNER have sync
	assert(SPAWNER == stack->bot->func_type); 
	assert(!(stack->in_user_code));
	stack->in_user_code = true;
	begin_strand(stack);

}

void ParasiteTool::call(const Event* e) {

	// F spawns or calls G:
	// G.w = 0
	// G.p = 0
	// G.l = 0
	// G.c = 0

	CallEvent* callEvent = (CallEvent*) e;
	const CallInfo *_info = callEvent->getCallInfo();
	const FUN_SG parentSignature = getSignature(currentThread->threadId);

    uint64_t strand_len = measure_and_add_strand_length(stack);
    if (stack->bot->c_head == stack->c_tail) {
      stack->bot->local_contin += strand_len;
    }

    // Push new frame for this C function onto the stack
    c_fn_frame_t *c_bottom = cilkprof_c_fn_push(stack);

    uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(rip);
    uintptr_t fn = (uintptr_t)this_fn;

    int32_t cs_index = add_to_iaddr_table(&call_site_table, cs, C_FUNCTION);
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

        fn_index = add_to_iaddr_table(&function_table, fn, C_FUNCTION);
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

    c_bottom->rip = (uintptr_t)__builtin_extract_return_addr(rip);
    c_bottom->function = (uintptr_t)this_fn;

    /* the stop time is also the start time of this function */
    // stack->start = stack->stop; /* TB: Want to exclude the length
    // (e.g. time or instruction count) of this function */
    begin_strand(stack);
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


    const c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);
  
    bool add_success;
    /* cilkprof_stack_frame_t *old_bottom; */
    const c_fn_frame_t *old_bottom;
  
    assert(stack->in_user_code);
  
    // stop the timer and attribute the elapsed time to this returning
    // function
    measure_and_add_strand_length(stack);
  
    assert(stack->c_tail > stack->bot->c_head);
  
    // Pop the stack
    old_bottom = cilkprof_c_fn_pop(stack);
    /* assert(old_bottom->local_wrk == old_bottom->local_contin); */
    uint64_t local_wrk = old_bottom->local_wrk;
    uint64_t running_wrk = old_bottom->running_wrk + local_wrk;
    uint64_t running_spn = old_bottom->running_spn + local_wrk;
  
    int32_t cs_index = old_bottom->cs_index;
    int32_t cs_tail = stack->cs_status[cs_index].c_tail;
    bool top_cs = (cs_tail == stack->c_tail + 1);
  
    if (top_cs) {  // top call site instance
      stack->cs_status[cs_index].c_tail = OFF_STACK;
      int32_t fn_index = stack->cs_status[cs_index].fn_index;
      if (stack->fn_status[fn_index] == stack->c_tail + 1) {
        stack->fn_status[fn_index] = OFF_STACK;
      }
    }
  
    c_fn_frame_t *new_bottom = &(stack->c_stack[stack->c_tail]);
    new_bottom->running_wrk += running_wrk;
    new_bottom->running_spn += running_spn;
  
  
    // Update work table
    if (top_cs) {
      uint32_t fn_index = stack->cs_status[new_bottom->cs_index].fn_index;
      /* fprintf(stderr, "adding to wrk table\n"); */
      add_success = add_to_cc_hashtable(&(stack->wrk_table),
                                        stack->c_tail == stack->fn_status[fn_index],
                                        cs_index,
                                        old_bottom->rip,
                                        running_wrk,
                                        running_spn,
                                        local_wrk,
                                        local_wrk);
      assert(add_success);

      add_success = add_to_cc_hashtable(&(stack->bot->contin_table),
                                        stack->c_tail == stack->fn_status[fn_index],
                                        cs_index,
                                        old_bottom->rip,
                                        running_wrk,
                                        running_spn,
                                        local_wrk,
                                        local_wrk);
      assert(add_success);
    } else {

      // Only record the local work and local span
      /* fprintf(stderr, "adding to wrk table\n"); */
      add_success = add_local_to_cc_hashtable(&(stack->wrk_table),
                                              cs_index,
                                              old_bottom->rip,
                                              local_wrk,
                                              local_wrk);
      assert(add_success);
      /* fprintf(stderr, "adding to contin table\n"); */
      add_success = add_local_to_cc_hashtable(&(stack->bot->contin_table),
                                              cs_index,
                                              old_bottom->rip,
                                              local_wrk,
                                              local_wrk);
      assert(add_success);
    }
  
    begin_strand(stack);

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

	cilkprof_stack_frame_t *old_bottom;
	bool add_success;

	uint64_t strand_len = measure_and_add_strand_length(stack);
	stack->bot->local_contin += strand_len;

	assert(stack->in_user_code);
	stack->in_user_code = false;

	// We are leaving this function, so it must have sync-ed, meaning
	// that, lchild should be 0 / empty.  prefix could contain value,
	// however, if this function is a Cilk function that spawned before.
	assert(0 == stack->bot->lchild_spn);
	assert(cc_hashtable_is_empty(stack->bot->lchild_table));

	assert(stack->bot->c_head == stack->c_tail);

	c_fn_frame_t *old_c_bottom = &(stack->c_stack[stack->c_tail]);

	stack->bot->prefix_spn += old_c_bottom->running_spn;
	stack->bot->local_spn += stack->bot->local_contin + BURDENING;
	old_c_bottom->running_wrk += old_c_bottom->local_wrk;
	stack->bot->prefix_spn += stack->bot->local_spn;

	add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->contin_table));

	// Pop the stack
	old_bottom = cilkprof_stack_pop(stack);

	c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

	int32_t cs_index = old_c_bottom->cs_index;
	int32_t cs_tail = stack->cs_status[cs_index].c_tail;
	bool top_cs = (cs_tail == stack->c_tail + 1);

	if (top_cs) {  // top CS instance
	stack->cs_status[cs_index].c_tail = OFF_STACK;
	int32_t fn_index = stack->cs_status[cs_index].fn_index;
	if (stack->fn_status[fn_index] == stack->c_tail + 1) {
	  stack->fn_status[fn_index] = OFF_STACK;
	}
	}

	c_bottom->running_wrk += old_c_bottom->running_wrk;

	// Update work table
	if (top_cs) {

		int32_t fn_index = stack->cs_status[c_bottom->cs_index].fn_index;
		add_success = add_to_cc_hashtable(&(stack->wrk_table),
		                                  stack->c_tail == stack->fn_status[fn_index],
		                                  cs_index,
		                                  old_c_bottom->rip,
		                                  old_c_bottom->running_wrk,
		                                  old_bottom->prefix_spn,
		                                  old_c_bottom->local_wrk,
		                                  old_bottom->local_spn);
		assert(add_success);


		add_success = add_to_cc_hashtable(&(old_bottom->prefix_table),
		                                  stack->c_tail == stack->fn_status[fn_index],
		                                  cs_index,
		                                  old_c_bottom->rip,
		                                  old_c_bottom->running_wrk,
		                                  old_bottom->prefix_spn,
		                                  old_c_bottom->local_wrk,
		                                  old_bottom->local_spn);
		assert(add_success);
	} 

	else {

		// Only record the local work and local span
		add_success = add_local_to_cc_hashtable(&(stack->wrk_table),
		                                        cs_index,
		                                        old_c_bottom->rip,
		                                        old_c_bottom->local_wrk,
		                                        old_bottom->local_spn);
		assert(add_success);

		add_success = add_local_to_cc_hashtable(&(old_bottom->prefix_table),
		                                        cs_index,
		                                        old_c_bottom->rip,
		                                        old_c_bottom->local_wrk,
		                                        old_bottom->local_spn);
		assert(add_success);
	}


	// This is the case we are returning to a spawn, since a HELPER 
	// is always invoked due to a spawn statement.

	assert(HELPER != stack->bot->func_type);

	if (c_bottom->running_spn + old_bottom->prefix_spn > stack->bot->lchild_spn) {

	  stack->bot->prefix_spn += c_bottom->running_spn;
	  stack->bot->local_spn += stack->bot->local_contin;

	  // This needs a better data structure to be implemented more
	  // efficiently.
	  add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->contin_table));

	  // Save old_bottom tables in new bottom's l_child variable.
	  stack->bot->lchild_spn = old_bottom->prefix_spn;
	  clear_cc_hashtable(stack->bot->lchild_table);

	  cc_hashtable_t* tmp_cc = stack->bot->lchild_table;
	  stack->bot->lchild_table = old_bottom->prefix_table;
	  old_bottom->prefix_table = tmp_cc;

	  // Free old_bottom tables that are no longer in use
	  clear_cc_hashtable(old_bottom->lchild_table);
	  clear_cc_hashtable(old_bottom->contin_table);

	  // Empy new bottom's continuation
	  c_bottom->running_spn = 0;
	  stack->bot->local_contin = 0;
	  clear_cc_hashtable(stack->bot->contin_table);

	} else {
	  // Discared all tables from old_bottom
	  clear_cc_hashtable(old_bottom->prefix_table);
	  clear_cc_hashtable(old_bottom->lchild_table);
	  clear_cc_hashtable(old_bottom->contin_table);

	}

	old_bottom->parent = stack->sf_free_list;
	stack->sf_free_list = old_bottom;
	begin_strand(stack);
}

