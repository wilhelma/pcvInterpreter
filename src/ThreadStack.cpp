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
	int bottom_index = stack.size() - 1;
	printf("popped off of thread stack, index is now %d \n", bottom_index);
}

void ThreadStack::init_frame(int thread_index, int head_function_index, 
											   TRD_ID thread_id,
											   vertex_descr_type first_vertex) {
	assert(thread_index >= 0);
	stack.at(thread_index)->head_function_index = head_function_index;
	stack.at(thread_index)->thread = thread_id;
	stack.at(thread_index)->first_vertex = first_vertex;
	stack.at(thread_index)->last_vertex = first_vertex;

}

std::shared_ptr<thread_frame_t> ThreadStack::push(int head_function_index, 
	 											  TRD_ID thread_id,
	 											  vertex_descr_type first_vertex) {
	stack.push_back(std::shared_ptr<thread_frame_t> (new thread_frame_t));
	int bottom_index = stack.size() - 1;
	printf("pushing on to thread stack, index is now %d \n", bottom_index);
	init_frame(bottom_index, head_function_index, thread_id, first_vertex);
	assert(bottom_index >= 0);
	return stack.at(bottom_index);
}





