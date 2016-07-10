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

FunctionStack::FunctionStack() {

	printf("function stack constructor has been called!!! \n");
	// // create empty function stack vector
	std::vector< std::shared_ptr<function_frame_t> > fxn_stack;
	stack = fxn_stack;
	bottom_index = -1;
}

FunctionStack::~FunctionStack() {

}

void FunctionStack::pop() {
	bottom_index -= 1;
	printf("popping off of function stack, index is now %d \n", bottom_index);
}

void FunctionStack::init_frame(int function_index, 
							   FUN_SG funSg,
							   CALLSITE callsiteID,
							   bool is_top_call_site_function) {

	stack.at(function_index)->local_work = 0;
	stack.at(function_index)->local_lock_span = 0;
	stack.at(function_index)->running_lock_span = 0;
	stack.at(function_index)->running_work = 0;
	stack.at(function_index)->running_span = 0;
	stack.at(function_index)->call_site = callsiteID;
	stack.at(function_index)->function_signature = funSg;
	stack.at(function_index)->is_top_call_site_function = is_top_call_site_function;
}

std::shared_ptr<function_frame_t> FunctionStack::push(FUN_SG funSg, 
									   CALLSITE callsiteID, 
									   bool is_top_call_site_function) {
	stack.push_back(std::shared_ptr<function_frame_t> (new function_frame_t));
	bottom_index += 1;
	init_frame(bottom_index, funSg, callsiteID, is_top_call_site_function);
	printf("pushing on to function stack, index is now %d \n", bottom_index);
	return stack.at(bottom_index);
}

