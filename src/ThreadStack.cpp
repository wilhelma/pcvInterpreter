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

#include <cassert>
#include "ThreadStack.h"

ThreadStack::ThreadStack() {
	// create empty thread stack vector
	std::vector< std::shared_ptr<thread_frame_t> > thrd_stack;
	stack = thrd_stack;
}

ThreadStack::~ThreadStack() {}

void ThreadStack::pop() {
	// the last thread frame must stay on the stack to get the end profile
	assert(stack.size() > 1);
	stack.pop_back();
}

void ThreadStack::init_frame(int thread_index, int head_function_index, 
											   TRD_ID thread_id) {
	assert(thread_index >= 0);
	stack.at(thread_index)->head_function_index = head_function_index;
	stack.at(thread_index)->thread = thread_id;
}

std::shared_ptr<thread_frame_t> ThreadStack::push(int head_function_index, 
	 											  TRD_ID thread_id) {
	stack.push_back(std::shared_ptr<thread_frame_t> (new thread_frame_t));
	int bottom_index = stack.size() - 1;
	init_frame(bottom_index, head_function_index, thread_id);
	assert(bottom_index >= 0);
	return stack.at(bottom_index);
}





