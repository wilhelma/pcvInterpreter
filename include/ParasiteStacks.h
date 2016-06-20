/*
 * ParasiteStacks.h
 *
 *  Created on: June 16, 2016
 *      Author: knapp
 */

#ifndef PARASITE_STACKS_H_
#define PARASITE_STACKS_H_

#include "Parasite.h"

struct function_frame_t {

	CALLSITE call_site;
	FUN_SG function_signature;

	// records whether function is a the first (top) function called from a call site
	bool is_top_call_site_function;


	// local work of this function 
	double local_work;

	// running work of this function's call site
	double running_work;

	// running span of this fucnction's call site
	double running_span;
};

struct thread_frame_t {

	// the ID of the thread with this data
	TRD_ID thread;

	// the index of the thread's head function (the function called when the thread spawns), 
	// in the function_stack variable of this frame's thread_stack_t
	int head_function_index;

	// Local continuation span of this thread's head function
	double local_continuation;

	// Local span of this thread's head function
	double local_span;

	// Span of the prefix of the thread's head function and its child functions
	double prefix_span;

	// Span of the longest spawned child of this thread's head function observed so
	// far
	double longest_child_span;

	// prefix data for each call_site in this thread 
	call_site_hashtable_t* prefix_table;

	// longest child data for each call_site in this thread 
	call_site_hashtable_t* longest_child_table;

	// continuation data for each call_site in this thread 
	call_site_hashtable_t* continuation_table;

	 // Pointer to the frame of this thread's parent thread
  	thread_frame_t *parent_thread;
};

struct main_stack_t {

  	// vector representing the stack of ALL functions
  	std::vector<function_frame_t*> function_stack;

  	// vector representing the stack of ALL threads 
  	std::vector<thread_frame_t*> thread_stack;

  	// running work data of all threads
  	call_site_hashtable_t *work_table;

  	int current_function_index;
  	int current_thread_index;

  	main_stack_t(){};
};


// FUNCTIONS OPERATING ON STACK
/*----------------------------------------------------------------------*/

static inline
void init_function_frame(function_frame_t *frame) {

	frame->local_work = 0;
	frame->running_work = 0;
	frame->running_span = 0;
}

static inline 
void init_thread_frame(thread_frame_t *frame, int head_function_index) {

	frame->head_function_index = head_function_index;
	frame->local_span = 0;
  	frame->local_continuation = 0;
	frame->prefix_span = 0; 
	frame->longest_child_span = 0;
}

static inline 
// push a new thread frame onto the thread stack and return reference to it 
thread_frame_t* thread_stack_push(main_stack_t *main_stack) {

	thread_frame_t* new_thread_frame = new thread_frame_t();
	main_stack->thread_stack.push_back(new_thread_frame);
	return new_thread_frame;
}

static inline 
// push a new function frame onto the function stack and return reference to it 
function_frame_t* function_stack_push(main_stack_t *main_stack) {

	function_frame_t* new_function_frame = new function_frame_t();
	main_stack->function_stack.push_back(new_function_frame);
	return new_function_frame;
}



#endif /* PARASITE_STACKS_H_ */

