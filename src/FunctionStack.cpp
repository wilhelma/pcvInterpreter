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
	print_debug("popped function");
}

void FunctionStack::init_frame(int index, 
							   FUN_SG funSg,
							   CALLSITE callsiteID,
							   int topCll,
							   int topCllOnThread) {
	assert(index >= 0);	
	stack.at(index)->call_site = callsiteID;
	stack.at(index)->function_signature = funSg;
	stack.at(index)->topCall = topCll;
	stack.at(index)->topCallOnThread = topCllOnThread;
}

std::shared_ptr<function_frame_t> FunctionStack::push(FUN_SG funSg, 
									   				  CALLSITE callsiteID,
									   				  int topCall,
									   				  int topCallOnThread) {
	stack.push_back(std::shared_ptr<function_frame_t> (new function_frame_t));
	print_debug("pushed function");
	int bottom_index = stack.size() - 1;
	init_frame(bottom_index, funSg, callsiteID, topCall, topCallOnThread);
	assert(bottom_index >= 0);
	return stack.back();
}

