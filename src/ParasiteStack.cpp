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

void ParasiteStack::function_stack_push() {
	function_stack.push_back(std::shared_ptr<function_frame_t> (new function_frame_t));
	current_function_index += 1;
	printf("pushing on to function stack, index is now %d \n", current_function_index);
}

void ParasiteStack::thread_stack_push() {
	thread_stack.push_back(std::shared_ptr<thread_frame_t> (new thread_frame_t));
	current_thread_index += 1;
	printf("pushing on to thread stack, index is now %d \n", current_thread_index);
}

void ParasiteStack::function_stack_pop() {
	current_function_index -= 1;
	printf("popping off of function stack, index is now %d \n", current_function_index);
}

void ParasiteStack::thread_stack_pop() {
	thread_stack.pop_back();
	current_thread_index -= 1;
	printf("popping off of thread stack, index is now %d \n", current_thread_index);
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

void ParasiteStack::init_function_frame(int function_index) {
	function_stack.at(function_index)->local_work = 0;
	function_stack.at(function_index)->local_lock_span = 0;
	function_stack.at(function_index)->running_lock_span = 0;
	function_stack.at(function_index)->running_work = 0;
	function_stack.at(function_index)->running_span = 0;
}

ParasiteStack::ParasiteStack() {

	// create empty function stack vector
	std::vector< std::shared_ptr<function_frame_t> > fxn_stack;

	// create empty thread stack vector
	std::vector< std::shared_ptr<thread_frame_t> > thrd_stack;

	// create empty call site hashtable
	std::shared_ptr<call_site_hashtable_t> wrk_table(new call_site_hashtable_t);

	function_stack = fxn_stack;
	thread_stack = thrd_stack;
	work_table = wrk_table;
	current_function_index = -1;
	current_thread_index = -1;

	// push main thread onto the stack
	thread_stack_push();
	init_thread_frame(0, 0);

	// push main function onto the stack
	function_stack_push();
	init_function_frame(0);

	printf("current function index is %d after creating tool \n", current_function_index);
}

ParasiteStack::~ParasiteStack() {

}




