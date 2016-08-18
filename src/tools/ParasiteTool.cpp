/**
 *
 *    @file  ParasiteTool.cpp
 *   @brief  Implementation file for the class `ParasiteTool`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#define COMMAND_LINE_OUTPUT 1
#define JSON_OUTPUT 1
#define GRAPH_OUTPUT 1

#include <algorithm>    // std::max
#include <cassert>
#include <string>
#include <utility>
#include <limits>
#include <climits>
#include "ParasiteTool.h"

ParasiteTool::ParasiteTool():last_thread_end_time(0), 
							 last_thread_runtime(0),
							 last_thread_start_time(0),
							 last_event_time(0) {

	stacks = std::unique_ptr<ParasiteTracker>(new ParasiteTracker());
	parasite_profile = std::unique_ptr<parasite_profile_t> (new parasite_profile_t);
	std::shared_ptr<CallSiteWorkHashtable> wrk_table(new CallSiteWorkHashtable());
	work_table = wrk_table;
}

vertex_descr_type ParasiteTool::add_edge(TIME length, std::string end_vertex_label) {

	vertex_descr_type new_vertex = 
		thread_graph.add_edge(stacks->bottomThread()->last_vertex, length,
							  end_vertex_label);
	stacks->bottomThread()->last_vertex = new_vertex;
	return new_vertex;
}

void ParasiteTool::add_join_edges(vertex_descr_type start) {

	add_edge(0, "J");
	thread_graph.add_join_edge(start, stacks->bottomThread()->last_vertex);
}

void ParasiteTool::endProfileCalculations() {

	assert(stacks->bottomThreadIndex() == 0);
	assert(stacks->bottomFunctionIndex() == 0);

	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();
	std::shared_ptr<function_frame_t> bottom_function = stacks->bottomFunction();
	parasite_profile->lock_wait_time = bottom_function->lock_wait_time();

	// Calculate span for entire program 
	parasite_profile->span = bottom_thread->prefix_span +
							 bottom_thread->continuation_span;
	print_time("bottom thread prefix span", bottom_thread->prefix_span);
	print_time("bottom thread local continuation",  bottom_thread->continuation_span);

	// Calculate work for entire program
	parasite_profile->work = bottom_function->running_work + bottom_function->local_work;
	print_time("bottom function local work",  bottom_function->local_work);
	print_time("bottom function running work",  bottom_function->running_work);

	// Calculate parallelism for entire program                     
	parasite_profile->parallelism =  static_cast<double> (parasite_profile->work)
				                   / static_cast<double> (parasite_profile->span);

	// bottom prefix table contains spans for all call sites at program end
	bottom_thread->prefix_table.add(&(bottom_thread->continuation_table));
}


void ParasiteTool::printOverallProfile() {

	std::cout << "PARALLELISM " << parasite_profile->parallelism << std::endl;
	print_time("WORK",  parasite_profile->work);
	print_time("SPAN",  parasite_profile->span);
	print_time("LOCK WAIT TIME",  parasite_profile->lock_wait_time);
	std::cout << "LOCK WAIT TIME IS " << 
						static_cast<double>(parasite_profile->lock_wait_time) /
						static_cast<double>(parasite_profile->span) <<
							                            " OF SPAN" << std::endl;
}

void ParasiteTool::printCallSiteProfiles() {

	// bottom prefix table contains spans for all call sites at program end
    CallSiteSpanHashtable bottom_prefix_table(stacks->bottomThread()->prefix_table);
	std::shared_ptr<call_site_work_hashtable_t> wrk_table = work_table->hashtable;
	for (auto const &it : *wrk_table) {
		CALLSITE key = it.first;
		if (bottom_prefix_table.hashtable->count(key)) {
			CallSiteProfile currentProfile(bottom_prefix_table.hashtable->at(key), 
									       wrk_table->at(key));
			currentProfile.print();
		}
	}
}

void ParasiteTool::printProfile() {

	std::cout << "================ \n" << std::endl;
	std::cout << "PRINT BEGIN \n" << std::endl;
	printCallSiteProfiles();
	printOverallProfile();
	std::cout << "PRINT END" << std::endl;
	std::cout << "================" << std::endl;
}

void ParasiteTool::writeJson() {

	std::shared_ptr<ParasiteJsonWriter> parasiteJsonWriter(new ParasiteJsonWriter());
	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();

	// Output json information for whole profile
	parasiteJsonWriter->writeOverallProfile(parasite_profile);

	// Output json information for each call site
    CallSiteSpanHashtable bottom_prefix_table(bottom_thread->prefix_table);
	std::shared_ptr<call_site_work_hashtable_t> wrk_table = work_table->hashtable;

	for (auto const &it : *wrk_table) {
		CALLSITE key = it.first;
		if (bottom_prefix_table.hashtable->count(key)) {
			std::shared_ptr<CallSiteProfile> currentProfile
					(new CallSiteProfile(bottom_prefix_table.hashtable->at(key),
										 wrk_table->at(key)));
			parasiteJsonWriter->writeCallSite(currentProfile);
		}
	}
}

ParasiteTool::~ParasiteTool() {

	std::cout << "Calling destructor" << std::endl;
	thread_graph.sink = thread_graph.last_vertex;
	endProfileCalculations();
	if (COMMAND_LINE_OUTPUT) 
		printProfile();
	if (JSON_OUTPUT) 
		writeJson();
	if (GRAPH_OUTPUT)
		thread_graph.write_dot_file();
}

void ParasiteTool::Call(const CallEvent* e) {
	std::cout << "================" << std::endl;
	std::cout << "CALL BEGIN " << std::endl;

	const CallInfo* _info(e->getCallInfo());
	FUN_SG calledFunctionSignature = _info->fnSignature;
	CALLSITE callsiteID = _info->siteId;
	last_event_time = _info->callTime;
	std::cout << "starting call Event with signature" <<
				      calledFunctionSignature.c_str() << std::endl;

	work_table->increment_count(callsiteID, calledFunctionSignature);
	stacks->function_push(calledFunctionSignature, callsiteID);
	
	std::cout << "CALL END " << std::endl;
	std::cout << "=============== " << std::endl;
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {
	std::cout << "=============== " << std::endl;
	std::cout << "NEW THREAD BEGIN " << std::endl;

	const NewThreadInfo* const _info = e->getNewThreadInfo();
	const TRD_ID newThreadID = _info->childThread->threadId;
	vertex_descr_type thread_start_vertex;

	// get information about the thread's head function
	TIME create_time = _info->startTime;
	last_event_time = create_time;
	last_thread_start_time = create_time;

	if (stacks->bottomThreadIndex() != -1) {
		std::shared_ptr<thread_frame_t> parent_thread = stacks->bottomThread();
		TIME local_work = create_time - last_event_time;
		assert(local_work >= 0);
		thread_start_vertex = add_edge(local_work, "TS");
		parent_thread->continuation_span += local_work;
		std::shared_ptr<function_frame_t> bottom_function = stacks->bottomFunction();
		parent_thread->continuation_table.add_span(bottom_function->call_site, 
												   local_work);
		bottom_function->local_work += local_work;
		work_table->add_work(bottom_function->call_site,
							 bottom_function->function_signature, 
							 local_work);
	} else {
		thread_start_vertex = thread_graph.last_vertex;
	}

	stacks->thread_push(stacks->bottomFunctionIndex(),     
		                newThreadID, 
		                thread_start_vertex);
	stacks->bottomThread()->thread_start_time = last_thread_start_time;

	std::cout << "NEW THREAD END " << std::endl;
	std::cout << "=============== " << std::endl;
}

void ParasiteTool::Join(const JoinEvent* e) {
	 
	std::cout << "=============== " << std::endl;
	std::cout << "STARTING JOIN " << std::endl;
	std::shared_ptr<thread_frame_t> bottom_thread(stacks->bottomThread());
	print_time("IN JOIN: longest child span",  bottom_thread->longest_child_span);
	print_time("IN JOIN: local continuation",  bottom_thread->continuation_span);

	add_join_edges(bottom_thread->join_vertex_list.front());
	bottom_thread->join_vertex_list.pop_front();


	TIME lock_wait_time_excluding_children = bottom_thread->lock_wait_time();
	bottom_thread->absorb_child_locks();
	TIME lock_wait_time_including_children = bottom_thread->lock_wait_time();
	TIME lock_wait_time_on_continuation = lock_wait_time_including_children - 
								   		  lock_wait_time_excluding_children;

	// If critical path goes through spawned child
	// If F.l > F.c
	if (bottom_thread->longest_child_span > bottom_thread->continuation_span) {

		// F.p += F.l
		bottom_thread->prefix_span += bottom_thread->longest_child_span;
		bottom_thread->prefix_table.add(&(bottom_thread->longest_child_table));

		// avoid double counting of lock wait time
		bottom_thread->prefix_span -= bottom_thread->longest_child_lock_wait_time;
		print_time("longest_child_lock_wait_time",
				 bottom_thread->longest_child_lock_wait_time);

		// local_span does not increase, because critical path goes 
		// through spawned child.

	} // F.c > F.l
		else {

		// Critical path goes through continuation, which is local. Add
		// continuation span to prefix span
		// F.p += F.c 
		bottom_thread->prefix_span += bottom_thread->continuation_span;
		bottom_thread->prefix_table.add(&(bottom_thread->continuation_table));

		// avoid double counting of lock wait time
		bottom_thread->prefix_span -= lock_wait_time_on_continuation;
		print_time("lock_wait_time_on_continuation",
				   lock_wait_time_on_continuation);
	}

	// reset longest child variables
	// F.c = 0
	bottom_thread->continuation_span = 0;
	bottom_thread->continuation_table.clear();
	// F.l = 0
	bottom_thread->longest_child_span = 0;
	bottom_thread->longest_child_table.clear();


	std::cout << "JOIN END " << std::endl;
	std::cout << "=============== " << std::endl;
}

// Called G returns to F
void ParasiteTool::Return(const ReturnEvent* e) {
	std::cout << "=============== " << std::endl;
	std::cout << "RETURN START " << std::endl;
	const ReturnInfo* _info(e->getReturnInfo());
	TIME returnTime = _info->endTime;
	assert(returnTime >= last_event_time);
	TIME local_work = returnTime - last_event_time;
	add_edge(local_work, "R");
	last_event_time = returnTime;
	print_time("last event time is now",  last_event_time);
	print_time("performing return operations for local work",  local_work);

	std::shared_ptr<function_frame_t> returned_function(stacks->bottomFunction());
	CALLSITE returning_call_site = returned_function->call_site;
	TIME returned_lock_wait_time = returned_function->lock_wait_time();
	returned_function->local_work += local_work + returned_lock_wait_time;
	print_time("returned_lock_wait_time", returned_lock_wait_time);

	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();
	bottom_thread->continuation_span += local_work;
	work_table->add_work(returning_call_site, 
						 returned_function->function_signature, 
						 local_work);

	CallSiteSpanHashtable bottom_thread_continuation_table(bottom_thread->
														   continuation_table);
	bottom_thread_continuation_table.set_lock_wait_time(returning_call_site, 
									                    returned_lock_wait_time);
	TIME running_work = returned_function->running_work + local_work;
	bottom_thread_continuation_table.add_span(returning_call_site, 
											  local_work);
	if (stacks->bottomFunctionIndex() == 0)
		return;

	std::shared_ptr<function_frame_t> parent_function = 
	 										     stacks->bottomParentFunction();
	// F.w += G.w
	parent_function->running_work += running_work;
	parent_function->add_locks(returned_function);
	work_table->add_work(parent_function->call_site,
	                     parent_function->function_signature,
	                     running_work);
	stacks->function_pop();
	std::cout << "RETURN END " << std::endl;
	std::cout << "=============== " << std::endl;
}

void ParasiteTool::ThreadEnd(const ThreadEndEvent* e) {
	std::cout << "=============== " << std::endl;
	std::cout << "THREAD END BEGIN " << std::endl;
	const ThreadEndInfo* _info(e->getThreadEndInfo());
	last_thread_end_time = _info->endTime;
	last_thread_runtime = _info->endTime - last_thread_start_time;
	TIME local_work = _info->endTime - last_event_time;
	add_edge(local_work, "TE");
	print_time("local work in thread end",  local_work);
	last_event_time = _info->endTime;
	print_time("last_event_time now",  last_event_time);

	std::shared_ptr<function_frame_t> current_function = stacks->bottomFunction();
	current_function->local_work += local_work;
	work_table->add_work(current_function->call_site,
						 current_function->function_signature, 
						 local_work);

	std::shared_ptr<thread_frame_t> ending_thread(stacks->bottomThread());
	ending_thread->continuation_span += local_work;
	ending_thread->continuation_table.add_span(current_function->call_site,
									    local_work);
	ending_thread->continuation_table.
						 set_lock_wait_time(current_function->call_site,
						 				    current_function->lock_wait_time());
	ending_thread->continuation_span += ending_thread->lock_wait_time();

	// G.p += G.c
	ending_thread->prefix_span += ending_thread->continuation_span;
	ending_thread->prefix_table.add(&(ending_thread->continuation_table));

	if (stacks->bottomThreadIndex() == 0) {
		return;
	}

	std::shared_ptr<thread_frame_t> parent_thread(stacks->bottomParentThread());
	parent_thread->join_vertex_list.push_back(ending_thread->last_vertex);
	parent_thread->concurrency_offset += last_thread_runtime;
	print_time("concurrency_offset now", parent_thread->concurrency_offset);

	// if the ending thread is the longest child encountered so far
	// F is parent thread, G is ending thread
	if (ending_thread->prefix_span + parent_thread->continuation_span 
			                       > parent_thread->longest_child_span) {

		std::cout << "ending thread is longest child encountered so far " << std::endl;

		// Save old bottom thread frame tables in 
		// parent frame's longest child variable.
		// F.l = G.p
		parent_thread->longest_child_span = ending_thread->prefix_span;
		parent_thread->longest_child_lock_wait_time = ending_thread->lock_wait_time(); 
		parent_thread->longest_child_table.clear();
		parent_thread->longest_child_table.add(&(ending_thread->prefix_table));

		// F.p += F.c
		parent_thread->prefix_table.add(&(parent_thread->continuation_table));
		parent_thread->prefix_span += parent_thread->continuation_span;

		// F.c = 0
		parent_thread->continuation_table.clear();
		parent_thread->continuation_span = 0;
	}

	parent_thread->add_child_locks(ending_thread);

	// pop the thread off the stack last, 
	// because the pop operation destroys the frame
	stacks->thread_pop();
	std::cout << "THREAD END EVENT END " << std::endl;
	std::cout << "=============== " << std::endl;
}

void ParasiteTool::Acquire(const AcquireEvent* e) {

	std::cout << "=============== " << std::endl;
	std::cout << "ACQUIRE EVENT BEGIN " << std::endl;

	const AcquireInfo* _info(e->getAcquireInfo());
	TIME acquire_time = _info->acquireTime;
	_info->lock->last_acquire_time = acquire_time;

	std::cout << "ACQUIRE EVENT END " << std::endl;
    std::cout << "=============== " << std::endl;
}

void ParasiteTool::Release(const ReleaseEvent* e) {

	std::cout << "=============== " << std::endl;
	std::cout << "RELEASE EVENT BEGIN " << std::endl;

	const ReleaseInfo* _info(e->getReleaseInfo());
	TIME release_time = _info->releaseTime;
	assert(static_cast<double>(release_time) >
		   static_cast<double>(_info->lock->last_acquire_time));
	
	unsigned int lockId = _info->lock->lockId;
	TIME offset = 0;
	if (stacks->bottomThreadIndex() > 0)
		offset = stacks->bottomParentThread()->concurrency_offset;

	stacks->bottomThread()->
		lock_intervals.addInterval(
					 _info->lock->last_acquire_time - offset, 
					 release_time - offset, lockId);
	stacks->bottomFunction()->
		lock_intervals.addInterval(
					 _info->lock->last_acquire_time - offset, 
					 release_time - offset, lockId);

	std::cout << "RELEASE EVENT END " << std::endl;
    std::cout << "=============== " << std::endl;
}

void ParasiteTool::Access(const AccessEvent* e) {
	std::cout << "ERROR: Parasite Tool does not implement access event"
																   << std::endl;
	return;
}




