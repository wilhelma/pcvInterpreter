/**
 *
 *    @file  ParasiteStack.cpp
 *   @brief  Implementation file for the class `ParasiteStack`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "ParasiteStack.h"

ParasiteStack::ParasiteStack() {
	// create empty function stack vector
	std::vector< std::shared_ptr<function_frame_t> > fxn_stack;

	// create empty thread stack vector
	std::vector< std::shared_ptr<thread_frame_t> > thrd_stack;

	// create empty call site hashtable
	std::shared_ptr<call_site_hashtable_t> wrk_table;

	function_stack = fxn_stack;
	thread_stack = thrd_stack;
	work_table = wrk_table;
	current_function_index = -1;
	current_thread_index = -1;
}

ParasiteStack::~ParasiteStack() {

}

void ParasiteStack::init_function_frame(int function_index) {
	function_stack.at(function_index)->local_work = 0;
	function_stack.at(function_index)->local_lock_span = 0;
	function_stack.at(function_index)->running_lock_span = 0;
	function_stack.at(function_index)->running_work = 0;
	function_stack.at(function_index)->running_span = 0;
}

void ParasiteStack::init_thread_frame(int thread_index, 
                                      int head_function_index) {
	thread_stack.at(thread_index)->head_function_index = head_function_index;
	thread_stack.at(thread_index)->local_span = 0;
	thread_stack.at(thread_index)->lock_span = 0;
  	thread_stack.at(thread_index)->local_continuation = 0;
	thread_stack.at(thread_index)->prefix_span = 0; 
	thread_stack.at(thread_index)->longest_child_span = 0;
	thread_stack.at(thread_index)->longest_child_lock_span = 0;
}

void ParasiteStack::thread_stack_push() {
	thread_stack.push_back(std::shared_ptr<thread_frame_t> (new thread_frame_t));
	current_thread_index += 1;
}

void ParasiteStack::function_stack_push() {
	std::shared_ptr<function_frame_t> new_function_frame(new function_frame_t);
	function_stack.push_back(new_function_frame);
	current_function_index += 1;
}
