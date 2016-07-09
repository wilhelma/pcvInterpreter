/**
 *
 *    @file  ParasiteTracker.cpp
 *   @brief  Implementation file for the class `ParasiteTracker`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include "ParasiteTracker.h"

ParasiteTracker::ParasiteTracker() {

	// create empty call site hashtable
	std::shared_ptr<call_site_hashtable_t> wrk_table(new call_site_hashtable_t);

	std::shared_ptr<FunctionStack> function_stack(new FunctionStack());
	std::shared_ptr<ThreadStack> thread_stack(new ThreadStack());
	work_table = wrk_table;
}

ParasiteTracker::~ParasiteTracker() {}


std::shared_ptr<function_frame_t> ParasiteTracker::function_push(FUN_SG funSg,
							   									 CALLSITE callsiteID,
							   									 bool is_top_call_site_function) {
	return function_stack->push(funSg, callsiteID, is_top_call_site_function);
}

void ParasiteTracker::function_pop(){
	assert(function_stack->bottom_index > 0);
	function_stack->pop();
}

std::shared_ptr<thread_frame_t> ParasiteTracker::thread_push(int head_function_index){
	return thread_stack->push(head_function_index);
}

void ParasiteTracker::thread_pop(){
	assert(thread_stack->bottom_index > 0);
	thread_stack->pop();
}

int ParasiteTracker::bottomThreadIndex() {
	return thread_stack->bottom_index;
}

int ParasiteTracker::bottomFunctionIndex() {
	return function_stack->bottom_index;
}

std::shared_ptr<thread_frame_t> ParasiteTracker::threadAt(int index) {
	assert(index >= 0);
	return thread_stack->stack.at(index);
}

std::shared_ptr<function_frame_t> ParasiteTracker::functionAt(int index) {
	assert(index >= 0);
	return function_stack->stack.at(index);
}

std::shared_ptr<thread_frame_t> ParasiteTracker::bottomThread() {
	assert(thread_stack->bottom_index >= 0);
	return threadAt(thread_stack->bottom_index);
}

std::shared_ptr<function_frame_t> ParasiteTracker::bottomFunction() {
	assert(thread_stack->bottom_index >= 0);
	return functionAt(function_stack->bottom_index);
}

std::shared_ptr<thread_frame_t> ParasiteTracker::bottomParentThread() {
	assert(thread_stack->bottom_index >= 0);
	if (thread_stack->bottom_index == 0)
		return threadAt(thread_stack->bottom_index);
	else 
		return threadAt(thread_stack->bottom_index - 1);
}

std::shared_ptr<function_frame_t> ParasiteTracker::bottomParentFunction() {
	assert(function_stack->bottom_index >= 0);
	if (function_stack->bottom_index == 0)
		return functionAt(thread_stack->bottom_index);
	else
		return functionAt(function_stack->bottom_index - 1);
}






