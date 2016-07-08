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

void ThreadStack::pop() {
	stack.pop_back();
	bottom_index -= 1;
	printf("popping off of thread stack, index is now %d \n", bottom_index);
}

void ThreadStack::init_frame(int thread_index, int head_function_index) {

	stack.at(thread_index)->head_function_index = head_function_index;
	stack.at(thread_index)->local_span = 0;
	stack.at(thread_index)->lock_span = 0;
  	stack.at(thread_index)->local_continuation = 0;
	stack.at(thread_index)->prefix_span = 0; 
	stack.at(thread_index)->longest_child_span = 0;
	stack.at(thread_index)->longest_child_lock_span = 0;
}

std::shared_ptr<thread_frame_t> ThreadStack::push(int head_function_index) {
	stack.push_back(std::shared_ptr<thread_frame_t> (new thread_frame_t));
	bottom_index += 1;
	printf("pushing on to thread stack, index is now %d \n", bottom_index);
	init_frame(bottom_index, head_function_index);
	return stack.at(bottom_index);
}

ThreadStack::ThreadStack() {

	// create empty thread stack vector
	std::vector< std::shared_ptr<thread_frame_t> > thrd_stack;

	stack = thrd_stack;
	bottom_index = -1;

	// push main thread onto the stack
	push(0);
	init_frame(0, 0);
}

ThreadStack::~ThreadStack() {

}




