/**
 *
 *    @file  ThreadStack.cpp
 *   @brief  Implementation file for the class `ThreadStack`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "ThreadStack.h"


void ThreadStack::thread_stack_push() {
	thread_stack.push_back(std::shared_ptr<thread_frame_t> (new thread_frame_t));
	current_thread_index += 1;
	printf("pushing on to thread stack, index is now %d \n", current_thread_index);
}

void ThreadStack::thread_stack_pop() {
	thread_stack.pop_back();
	current_thread_index -= 1;
	printf("popping off of thread stack, index is now %d \n", current_thread_index);
}

void ThreadStack::init_thread_frame(int thread_index, 
                                      int head_function_index) {
	thread_stack.at(thread_index)->head_function_index = head_function_index;
	thread_stack.at(thread_index)->local_span = 0;
	thread_stack.at(thread_index)->lock_span = 0;
  	thread_stack.at(thread_index)->local_continuation = 0;
	thread_stack.at(thread_index)->prefix_span = 0; 
	thread_stack.at(thread_index)->longest_child_span = 0;
	thread_stack.at(thread_index)->longest_child_lock_span = 0;
}


ThreadStack::ThreadStack() {

	// create empty thread stack vector
	std::vector< std::shared_ptr<thread_frame_t> > thrd_stack;

	// create empty call site hashtable
	std::shared_ptr<call_site_hashtable_t> wrk_table(new call_site_hashtable_t);

	thread_stack = thrd_stack;
	work_table = wrk_table;
	current_thread_index = -1;

	// push main thread onto the stack
	thread_stack_push();
	init_thread_frame(0, 0);
}

ThreadStack::~ThreadStack() {

}




