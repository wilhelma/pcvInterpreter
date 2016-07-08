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

#include "ParasiteTracker.h"


ParasiteTracker::ParasiteTracker() {

	// create empty call site hashtable
	std::shared_ptr<call_site_hashtable_t> wrk_table(new call_site_hashtable_t);

	std::shared_ptr<FunctionStack> function_stack(new FunctionStack());
	std::shared_ptr<ThreadStack> thread_stack(new ThreadStack());
	work_table = wrk_table;
}

ParasiteTracker::~ParasiteTracker() {}


int bottomFunctionIndex() {
	return function_stack->bottom_function_index;
}

std::shared_ptr<thread_frame_t> ParasiteTracker::threadAt(int index) {
	return thread_stack->stack.at(index);
}

std::shared_ptr<thread_frame_t> ParasiteTracker::functionAt(int index) {
	return function_stack->stack.at(index);
}

std::shared_ptr<thread_frame_t> ParasiteTracker::bottomThread() {
	return threadAt(thread_stack->bottom_thread_index);
}

std::shared_ptr<function_frame_t> ParasiteTracker::bottomFunction() {
	return functionAt(function_stack->bottom_function_index);
}

std::shared_ptr<thread_frame_t> ParasiteTracker::bottomParentThread() {
	return threadAt(thread_stack->bottom_thread_index - 1);
}

std::shared_ptr<function_frame_t> ParasiteTracker::bottomParentFunction() {
	return functionAt(function_stack->bottom_function_index - 1);
}






