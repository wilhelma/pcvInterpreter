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

    parasite_tool_print(main_stack);
    parasite_stack_frame_t *old_bottom = stack->bot;
    free_parasite_hashtable(main_stack->work_table);

    old_bottom->parent = stack->stack_frame_free_list;
    stack->stack_frame_free_list = old_bottom;

    parasite_stack_frame_t *free_frame = stack->stack_frame_free_list;
    parasite_stack_frame_t *next_free_frame;

    while (NULL != free_frame) {

      next_free_frame = free_frame->parent;
      free_parasite_hashtable(free_frame->prefix_table);
      free_parasite_hashtable(free_frame->longest_child_table);
      free_parasite_hashtable(free_frame->continuation_table);
      free(free_frame);
      free_frame = next_free_frame;
    }

    stack->stack_frame_free_list = NULL;

    free(main_stack->call_site_status);
    free(main_stack->function_status);
    free(main_stack->function_stack);

    parasite_hashtable_linked_list_node_t *free_list_node = linked_list_free_list;
    parasite_hashtable_linked_list_node_t *next_free_list_node;

    while (NULL != free_list_node) {
      next_free_list_node = free_list_node->next;
      free(free_list_node);
      free_list_node = next_free_list_node;
    }
    linked_list_free_list = NULL;

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
	main_stack->bottom->local_continuation += strand_len;
	assert(main_stack->tailFunctionSignature == main_stack->bottom->headFunctionSignature);
	assert(main_stack->tailFunctionSignature == stack->bottom->headFunctionSignature);
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
	stack->bottom->local_contin += strand_len;

	assert(main_stack->bottom->headFunctionSignature == stack->tailFunctionSignature);

	cilkprof_stack_t *stack = &(GET_STACK(ctx_stack));

	assert(main_stack->bottom->headFunctionSignature == stack->tailFunctionSignature);

	function_frame_t *c_bottom = &(main_stack->function_stack[stack->tailFunctionSignature]);

	c_bottom->running_span += stack->bottom->local_continuation;

	if (main_stack->bottom->longest_child_span > c_bottom->running_span) {
	stack->bottom->prefix_span += stack->bottom->longest_child_span;

	// local_span does not increase, because critical path goes through
	// spawned child.
	add_parasite_hashtables(&(main_stack->bottom->prefix_table), &(main_stack->bottom->longest_child_table));

	} else {
	/* stack->bottom->prefix_span += stack->bottom->continuation_span; */
	stack->bottom->prefix_span += c_bottom->running_span;
	// critical path goes through continuation, which is local.  add
	// local_contin to local_span.
	stack->bottom->local_span += stack->bottom->local_continuation;
	add_parasite_hashtables(&(main_stack->bottom->prefix_table), &(main_stack->bottom->continuation_table));

	}

	// reset lchild and contin span variables
	stack->bottom->longest_child_span = 0;
	c_bottom->running_span = 0;
	stack->bottom->local_contin = 0;
	clear_parasite_hashtable(main_stack->bottom->longest_child_table);
	clear_parasite_hashtable(main_stack->bottom->continuation_table);

	// can't be anything else; only SPAWNER have sync
	assert(SPAWNER == stack->bottom->func_type); 
	assert(!(main_stack->in_user_code));
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
    if (main_stack->bottom->headFunctionSignature == stack->tailFunctionSignature) {
      stack->bottom->local_contin += strand_len;
    }

    // Push new frame for this C function onto the stack
    function_frame_t *c_bottom = cilkprof_c_fn_push(stack);

    uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(call_site_ID);
    uintptr_t fn = (uintptr_t)this_fn;

    int32_t call_site_ID = add_to_iaddr_table(&call_site_table, cs, C_FUNCTION);
    c_bottom->call_site_ID = call_site_ID;
    if (call_site_ID >= stack->cs_status_capacity) {
      resize_cs_status_vector(&(main_stack->cs_status), &(main_stack->cs_status_capacity));
    }
    int32_t cs_tail = stack->cs_status[call_site_ID].tailFunctionSignature;
    if (OFF_STACK != cs_tail) {
      if (!(main_stack->cs_status[call_site_ID].flags & RECURSIVE)) {
        stack->cs_status[call_site_ID].flags |= RECURSIVE;
      }
    } else {
      int32_t fn_index;
      if (UNINITIALIZED == stack->cs_status[call_site_ID].fn_index) {

        assert(call_site_table->table_size == call_site_ID + 1);
        MIN_CAPACITY = call_site_ID + 1;

        fn_index = add_to_iaddr_table(&function_table, fn, C_FUNCTION);
        stack->cs_status[call_site_ID].fn_index = fn_index;
        if (fn_index >= stack->fn_status_capacity) {
          resize_fn_status_vector(&(main_stack->fn_status), &(main_stack->fn_status_capacity));
        }
      } else {
        fn_index = stack->cs_status[call_site_ID].fn_index;
      }
      stack->cs_status[call_site_ID].tailFunctionSignature = stack->tailFunctionSignature;
      if (OFF_STACK == stack->fn_status[fn_index]) {
        stack->fn_status[fn_index] = stack->tailFunctionSignature;
      }
    }

    c_bottom->call_site_ID = (uintptr_t)__builtin_extract_return_addr(call_site_ID);
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

    const function_frame_t *c_bottom = &(main_stack->function_stack[stack->tailFunctionSignature]);
  
    bool add_success;

    const function_frame_t *old_bottom;
  
    assert(main_stack->in_user_code);
  
    // stop the timer and attribute the elapsed time to this returning
    // function
    measure_and_add_strand_length(stack);
  
    assert(main_stack->tailFunctionSignature > stack->bottom->headFunctionSignature);
  
    // Pop the stack
    old_bottom = cilkprof_c_fn_pop(stack);
    uint64_t local_work = old_bottom->local_work;
    uint64_t running_work = old_bottom->running_work + local_work;
    uint64_t running_span = old_bottom->running_span + local_work;
  
    int32_t call_site_ID = old_bottom->call_site_ID;
    int32_t cs_tail = stack->cs_status[call_site_ID].tailFunctionSignature;
    bool top_cs = (cs_tail == stack->tailFunctionSignature + 1);
  
    if (top_cs) {  // top call site instance
      stack->cs_status[call_site_ID].tailFunctionSignature = OFF_STACK;
      int32_t fn_index = stack->cs_status[call_site_ID].fn_index;
      if (main_stack->fn_status[fn_index] == stack->tailFunctionSignature + 1) {
        stack->fn_status[fn_index] = OFF_STACK;
      }
    }
  
    function_frame_t *new_bottom = &(main_stack->function_stack[stack->tailFunctionSignature]);
    new_bottom->running_work += running_work;
    new_bottom->running_span += running_span;
  
  
    // Update work table
    if (top_cs) {
      uint32_t fn_index = stack->cs_status[new_bottom->call_site_ID].fn_index;
      /* fprintf(stderr, "adding to work table\n"); */
      add_success = add_to_parasite_hashtable(&(main_stack->work_table),
                                        stack->tailFunctionSignature == stack->fn_status[fn_index],
                                        call_site_ID,
                                        old_bottom->call_site_ID,
                                        running_work,
                                        running_span,
                                        local_work,
                                        local_work);
      assert(add_success);

      add_success = add_to_parasite_hashtable(&(main_stack->bottom->continuation_table),
                                        stack->tailFunctionSignature == stack->fn_status[fn_index],
                                        call_site_ID,
                                        old_bottom->call_site_ID,
                                        running_work,
                                        running_span,
                                        local_work,
                                        local_work);
      assert(add_success);
    } else {

      // Only record the local work and local span
      /* fprintf(stderr, "adding to work table\n"); */
      add_success = add_local_to_parasite_hashtable(&(main_stack->work_table),
                                              call_site_ID,
                                              old_bottom->call_site_ID,
                                              local_work,
                                              local_work);
      assert(add_success);
      /* fprintf(stderr, "adding to contin table\n"); */
      add_success = add_local_to_parasite_hashtable(&(main_stack->bottom->continuation_table),
                                              call_site_ID,
                                              old_bottom->call_site_ID,
                                              local_work,
                                              local_work);
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
	stack->bottom->local_contin += strand_len;

	assert(main_stack->in_user_code);
	stack->in_user_code = false;

	// We are leaving this function, so it must have sync-ed, meaning
	// that, lchild should be 0 / empty.  prefix could contain value,
	// however, if this function is a Cilk function that spawned before.
	assert(0 == stack->bottom->longest_child_span);
	assert(parasite_hashtable_is_empty(main_stack->bottom->longest_child_table));

	assert(main_stack->bottom->headFunctionSignature == stack->tailFunctionSignature);

	function_frame_t *old_c_bottom = &(main_stack->function_stack[stack->tailFunctionSignature]);

	stack->bottom->prefix_span += old_c_bottom->running_span;
	stack->bottom->local_span += stack->bottom->local_contin + BURDENING;
	old_c_bottom->running_work += old_c_bottom->local_work;
	stack->bottom->prefix_span += stack->bottom->local_span;

	add_parasite_hashtables(&(main_stack->bottom->prefix_table), &(main_stack->bottom->continuation_table));

	// Pop the stack
	old_bottom = cilkprof_stack_pop(stack);

	function_frame_t *c_bottom = &(main_stack->function_stack[stack->tailFunctionSignature]);

	int32_t call_site_ID = old_c_bottom->call_site_ID;
	int32_t cs_tail = stack->cs_status[call_site_ID].tailFunctionSignature;
	bool top_cs = (cs_tail == stack->tailFunctionSignature + 1);

	if (top_cs) {  // top CS instance
	stack->cs_status[call_site_ID].tailFunctionSignature = OFF_STACK;
	int32_t fn_index = stack->cs_status[call_site_ID].fn_index;
	if (main_stack->fn_status[fn_index] == stack->tailFunctionSignature + 1) {
	  stack->fn_status[fn_index] = OFF_STACK;
	}
	}

	c_bottom->running_work += old_c_bottom->running_work;

	// Update work table
	if (top_cs) {

		int32_t fn_index = stack->cs_status[c_bottom->call_site_ID].fn_index;
		add_success = add_to_parasite_hashtable(&(main_stack->work_table),
		                                  stack->tailFunctionSignature == stack->fn_status[fn_index],
		                                  call_site_ID,
		                                  old_c_bottom->call_site_ID,
		                                  old_c_bottom->running_work,
		                                  old_bottom->prefix_span,
		                                  old_c_bottom->local_work,
		                                  old_bottom->local_span);
		assert(add_success);


		add_success = add_to_parasite_hashtable(&(old_bottom->prefix_table),
		                                  stack->tailFunctionSignature == stack->fn_status[fn_index],
		                                  call_site_ID,
		                                  old_c_bottom->call_site_ID,
		                                  old_c_bottom->running_work,
		                                  old_bottom->prefix_span,
		                                  old_c_bottom->local_work,
		                                  old_bottom->local_span);
		assert(add_success);
	} 

	else {

		// Only record the local work and local span
		add_success = add_local_to_parasite_hashtable(&(main_stack->work_table),
		                                        call_site_ID,
		                                        old_c_bottom->call_site_ID,
		                                        old_c_bottom->local_work,
		                                        old_bottom->local_span);
		assert(add_success);

		add_success = add_local_to_parasite_hashtable(&(old_bottom->prefix_table),
		                                        call_site_ID,
		                                        old_c_bottom->call_site_ID,
		                                        old_c_bottom->local_work,
		                                        old_bottom->local_span);
		assert(add_success);
	}


	// This is the case we are returning to a spawn, since a HELPER 
	// is always invoked due to a spawn statement.

	assert(HELPER != stack->bottom->func_type);

	if (c_bottom->running_span + old_bottom->prefix_span > stack->bottom->longest_child_span) {

	  stack->bottom->prefix_span += c_bottom->running_span;
	  stack->bottom->local_span += stack->bottom->local_continuation;

	  // This needs a better data structure to be implemented more
	  // efficiently.
	  add_parasite_hashtables(&(main_stack->bottom->prefix_table), &(main_stack->bottom->continuation_table));

	  // Save old_bottom tables in new bottom's l_child variable.
	  stack->bottom->longest_child_span = old_bottom->prefix_span;
	  clear_parasite_hashtable(main_stack->bottom->longest_child_table);

	  parasite_hashtable_t* tmp_hashtable = stack->bottom->longest_child_table;
	  stack->bottom->longest_child_table = old_bottom->prefix_table;
	  old_bottom->prefix_table = tmp_hashtable;

	  // Free old_bottom tables that are no longer in use
	  clear_parasite_hashtable(old_bottom->longest_child_table);
	  clear_parasite_hashtable(old_bottom->continuation_table);

	  // Empy new bottom's continuation
	  c_bottom->running_span = 0;
	  stack->bottom->local_contin = 0;
	  clear_parasite_hashtable(main_stack->bottom->continuation_table);

	} else {
	  // Discared all tables from old_bottom
	  clear_parasite_hashtable(old_bottom->prefix_table);
	  clear_parasite_hashtable(old_bottom->longest_child_table);
	  clear_parasite_hashtable(old_bottom->continuation_table);

	}

	old_bottom->parent = stack->stack_frame_free_list;
	stack->stack_frame_free_list = old_bottom;
	begin_strand(stack);
}

