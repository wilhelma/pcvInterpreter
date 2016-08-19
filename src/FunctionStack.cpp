/**
 *
 *    @file  FunctionStack.cpp
 *   @brief  Implementation file for the class `FunctionStack`
 *
 *    @date  (TIME) 06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include <memory> 
#include "FunctionStack.h"

FunctionStack::FunctionStack() {
	// create empty function stack vector
	std::vector< std::shared_ptr<function_frame_t> > fxn_stack;
	stack = fxn_stack;
}

FunctionStack::~FunctionStack() {}

void FunctionStack::pop() {

	// the last function frame must stay on the stack to get the end profile
	assert(stack.size() > 1);
	stack.pop_back();
	int bottom_index = stack.size() - 1;
	if (DEBUG_OUTPUT)
		std::cout << "popped off of function stack, index is now " << bottom_index << std::endl;
}

void FunctionStack::init_frame(int index, 
							   FUN_SG funSg,
							   CALLSITE callsiteID) {
	assert(index >= 0);	
	stack.at(index)->call_site = callsiteID;
	stack.at(index)->function_signature = funSg;
}

std::shared_ptr<function_frame_t> FunctionStack::push(FUN_SG funSg, 
									   				  CALLSITE callsiteID) {
	stack.push_back(std::shared_ptr<function_frame_t> (new function_frame_t));
	int bottom_index = stack.size() - 1;
	init_frame(bottom_index, funSg, callsiteID);
	if (DEBUG_OUTPUT)
		std::cout << "pushing on to function stack, index is now " << bottom_index << std::endl;
	assert(bottom_index >= 0);
	return stack.back();
}

