/*
 * ParasiteStack.h
 *
 *  Created on: June 16, 2016
 *      Author: knapp
 */

#ifndef PARASITE_STACK_H_
#define PARASITE_STACK_H_

struct function_frame_t {

	CALLSITE call_site;
	FUN_SG function_signature;

	// records whether function is a the first (top) function called from a call site
	bool is_top_call_site_function;

	// local work of this function 
	double local_work;

	// local lock span of this function
	double local_lock_span;

	// running work of this function's call site
	double running_work;

	// running lock span of this function's call site
	double running_lock_span;

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

	// Local lock span of this thread's head function
	double local_lock_span;

	// Span of the prefix of the thread's head function and its child functions
	double prefix_span;

	// Lock span of this thread
	double lock_span;

	// Span of the longest spawned child of this thread's head function observed so
	// far
	double longest_child_span;

	// Lock span of the longest spawned child of this thread's head function observed so
	// far
	double longest_child_lock_span;

	// prefix data for each call_site in this thread 
	// call_site_hashtable_t* prefix_table;
	std::unique_ptr<call_site_hashtable_t> prefix_table;

	// longest child data for each call_site in this thread 
	// call_site_hashtable_t* longest_child_table;
	std::unique_ptr<call_site_hashtable_t> longest_child_table;

	// continuation data for each call_site in this thread 
	// call_site_hashtable_t* continuation_table;
	std::unique_ptr<call_site_hashtable_t> continuation_table;
};

class ParasiteStack {

	public:

		ParasiteStack();
		~ParasiteStack();

		void init_function_frame(int function_index);
		void init_thread_frame(int thread_index, int head_function_index);
		void function_stack_push();
		void thread_stack_push();
		
		// vector representing the stack of ALL functions
  		std::vector< std::unique_ptr<function_frame_t> > function_stack;

  		// vector representing the stack of ALL threads 
  		std::vector< std::unique_ptr<function_frame_t> > thread_stack;

  		// running work data of all threads
  		std::unique_ptr<call_site_hashtable_t> work_table;

  		int current_function_index;
  		int current_thread_index;

  	private:

  		ParasiteStack(const ParasiteStack&);
		ParasiteStack& operator=(const ParasiteStack&);
}


#endif // PARASITE_H_ 
