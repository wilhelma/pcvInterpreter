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

	std::shared_ptr<call_site_work_hashtable_t> wrk_table(new call_site_work_hashtable_t);
	function_stack = std::unique_ptr<FunctionStack>(new FunctionStack());
	thread_stack = std::unique_ptr<ThreadStack>(new ThreadStack());
}

ParasiteTracker::~ParasiteTracker() {}

std::shared_ptr<function_frame_t> ParasiteTracker::function_push(FUN_SG funSg,
							   									 CALLSITE callsiteID,
							   									 int topCall,
							   									 int topCallOnThread) {
	return function_stack->push(funSg, callsiteID, topCall, topCallOnThread);
}

void ParasiteTracker::function_pop(){
	function_stack->pop();
}

std::shared_ptr<thread_frame_t> ParasiteTracker::thread_push(int head_function_index,
															 TRD_ID thread_id,
															 vertex_descr_type first_vertex) {
	return thread_stack->push(head_function_index, thread_id, first_vertex);
}

void ParasiteTracker::thread_pop(){
	thread_stack->pop();
}

int ParasiteTracker::bottomThreadIndex() {
	return (thread_stack->stack.size() - 1);
}

int ParasiteTracker::bottomFunctionIndex() {
	return (function_stack->stack.size() - 1);
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
	return thread_stack->stack.back();
}

std::shared_ptr<function_frame_t> ParasiteTracker::bottomFunction() {
	return function_stack->stack.back();
}

std::shared_ptr<thread_frame_t> ParasiteTracker::bottomParentThread() {
	return threadAt(thread_stack->stack.size() - 2);
}

std::shared_ptr<function_frame_t> ParasiteTracker::bottomParentFunction() {
	return functionAt(function_stack->stack.size() - 2);
}






