/**
 *
 *    @file  FunctionStack.cpp
 *   @brief  Implementation file for the class `FunctionStack`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "FunctionStack.h"

void FunctionStack::function_stack_push() {
	function_stack.push_back(std::shared_ptr<function_frame_t> (new function_frame_t));
	current_function_index += 1;
	printf("pushing on to function stack, index is now %d \n", current_function_index);
}

void FunctionStack::function_stack_pop() {
	current_function_index -= 1;
	printf("popping off of function stack, index is now %d \n", current_function_index);
}

void FunctionStack::init_function_frame(int function_index) {
	function_stack.at(function_index)->local_work = 0;
	function_stack.at(function_index)->local_lock_span = 0;
	function_stack.at(function_index)->running_lock_span = 0;
	function_stack.at(function_index)->running_work = 0;
	function_stack.at(function_index)->running_span = 0;
}

FunctionStack::FunctionStack() {

	// create empty function stack vector
	std::vector< std::shared_ptr<function_frame_t> > fxn_stack;

	function_stack = fxn_stack;
	current_function_index = -1;

	// push main function onto the stack
	function_stack_push();
	init_function_frame(0);
}

FunctionStack::~FunctionStack() {

}




