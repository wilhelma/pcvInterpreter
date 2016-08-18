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

ParasiteTool::ParasiteTool():last_thread_start_time(0),
							 last_event_time(0) {

	stacks = std::unique_ptr<ParasiteTracker>(new ParasiteTracker());
	parasite_profile = std::unique_ptr<parasite_profile_t>(new parasite_profile_t);
	work = std::shared_ptr<Work>(new Work());
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

void ParasiteTool::print_event_start(std::string event_name) {

	if (DEBUG_OUTPUT) {
		std::cout << "==================" << std::endl;
		std::cout << event_name << " BEGIN" << std::endl;
	}
}

void ParasiteTool::print_event_end(std::string event_name) {

	if (DEBUG_OUTPUT) {
		std::cout << event_name << " END" << std::endl;
		std::cout << "==================" << std::endl;
	}
}


vertex_descr_type ParasiteTool::add_local_work(TIME strand_end_time, 
	  						      std::string end_vertex_label) {

	print_event_start(end_vertex_label);
	TIME local_work = strand_end_time - last_event_time;
	last_event_time = strand_end_time;
	assert(local_work >= 0);
	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();
	std::shared_ptr<function_frame_t> bottom_function = stacks->bottomFunction();
	work->add_to_call_site(bottom_function->call_site,
						   bottom_function->function_signature, 
						   local_work);
	bottom_thread->continuation.add_to_call_site(bottom_function->call_site,
									    	    local_work);
	print_time("local work", local_work);
	print_time("last_event_time", last_event_time);
	return add_edge(local_work, end_vertex_label);
}



void ParasiteTool::endProfileCalculations() {

	assert(stacks->bottomThreadIndex() == 0);
	assert(stacks->bottomFunctionIndex() == 0);

	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();
	std::shared_ptr<function_frame_t> bottom_function = stacks->bottomFunction();
	parasite_profile->lock_wait_time = bottom_function->lock_wait_time();

	// Calculate span for entire program 
	bottom_thread->prefix.add(&(bottom_thread->continuation));

	// Calculate work for entire program
	parasite_profile->work = bottom_function->running_work + bottom_function->local_work;

	// Calculate parallelism for entire program                     
	parasite_profile->parallelism =  static_cast<double> (parasite_profile->work)
				                   / static_cast<double> (parasite_profile->span);
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
    Span bottom_prefix(stacks->bottomThread()->prefix);
	std::shared_ptr<call_site_work_hashtable_t> wrk = work->hashtable;
	for (auto const &it : *wrk) {
		CALLSITE key = it.first;
		if (bottom_prefix.hashtable->count(key)) {
			CallSiteProfile currentProfile(bottom_prefix.hashtable->at(key), 
									       wrk->at(key));
			currentProfile.print();
		}
	}
}

void ParasiteTool::writeJson() {

	std::shared_ptr<ParasiteJsonWriter> parasiteJsonWriter(new ParasiteJsonWriter());
	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();

	// Output json information for whole profile
	parasiteJsonWriter->writeOverallProfile(parasite_profile);

	// Output json information for each call site
    Span bottom_prefix(bottom_thread->prefix);
	std::shared_ptr<call_site_work_hashtable_t> wrk = work->hashtable;

	for (auto const &it : *wrk) {
		CALLSITE key = it.first;
		if (bottom_prefix.hashtable->count(key)) {
			std::shared_ptr<CallSiteProfile> currentProfile
					(new CallSiteProfile(bottom_prefix.hashtable->at(key),
										 wrk->at(key)));
			parasiteJsonWriter->writeCallSite(currentProfile);
		}
	}
}

ParasiteTool::~ParasiteTool() {

	std::cout << "Calling destructor" << std::endl;
	thread_graph.sink = thread_graph.last_vertex;
	endProfileCalculations();
	if (COMMAND_LINE_OUTPUT) {
		printCallSiteProfiles();
		printOverallProfile();
	}
	if (JSON_OUTPUT) 
		writeJson();
	if (GRAPH_OUTPUT)
		thread_graph.write_dot_file();
}

void ParasiteTool::Call(const CallEvent* e) {

	print_event_start("CALL");
	const CallInfo* _info(e->getCallInfo());
	FUN_SG calledFunctionSignature = _info->fnSignature;
	CALLSITE callsiteID = _info->siteId;
	last_event_time = _info->callTime;
	std::cout << "starting call Event with signature " <<
				      calledFunctionSignature.c_str() << std::endl;

	work->increment_count(callsiteID, calledFunctionSignature);
	stacks->function_push(calledFunctionSignature, callsiteID);
	print_event_end("CALL");
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {

	const NewThreadInfo* const _info = e->getNewThreadInfo();
	vertex_descr_type thread_start_vertex;

	if (stacks->bottomThreadIndex() != -1) 
		thread_start_vertex = add_local_work(_info->startTime, "TS");
	else 
		thread_start_vertex = thread_graph.last_vertex;

	stacks->thread_push(stacks->bottomFunctionIndex(),     
		                _info->childThread->threadId, 
		                thread_start_vertex);
	stacks->bottomThread()->thread_start_time = _info->startTime;

	print_event_end("NEW THREAD");
}

void ParasiteTool::Join(const JoinEvent* e) {
	 
	print_event_start("JOIN");
	std::shared_ptr<thread_frame_t> bottom_thread(stacks->bottomThread());
	add_join_edges(bottom_thread->join_vertex_list.front());
	bottom_thread->join_vertex_list.pop_front();

	TIME lock_wait_time_excluding_children = bottom_thread->lock_wait_time();
	bottom_thread->absorb_child_locks();
	TIME lock_wait_time_including_children = bottom_thread->lock_wait_time();
	TIME lock_wait_time_on_continuation = lock_wait_time_including_children - 
								   		  lock_wait_time_excluding_children;

	// If critical path goes through spawned child
	// If F.l > F.c
	if (bottom_thread->longest_child() > bottom_thread->continuation()) {

		// F.p += F.l
		bottom_thread->prefix.add(&(bottom_thread->longest_child));

		// avoid double counting of lock wait time
		bottom_thread->
					correct_prefix(bottom_thread->longest_child_lock_wait_time);

		// local() does not increase, because critical path goes 
		// through spawned child.

	} // F.c > F.l
		else {

		// Critical path goes through continuation, which is local. Add
		// continuation span to prefix span
		// F.p += F.c 
		bottom_thread->prefix.add(&(bottom_thread->continuation));

		// avoid double counting of lock wait time
		bottom_thread->correct_prefix(lock_wait_time_on_continuation);
	}

	// reset longest child variables
	// F.c = 0
	bottom_thread->continuation.clear();

	// F.l = 0
	bottom_thread->longest_child.clear();

	print_event_end("JOIN");
}

// Called G returns to F
void ParasiteTool::Return(const ReturnEvent* e) {

	const ReturnInfo* _info(e->getReturnInfo());
	add_local_work(_info->endTime, "R");

	std::shared_ptr<function_frame_t> returned_function(stacks->bottomFunction());
	TIME returned_lock_wait_time = returned_function->lock_wait_time();

	std::shared_ptr<thread_frame_t> bottom_thread = stacks->bottomThread();
	returned_function->local_work += returned_lock_wait_time;
	bottom_thread->continuation.set_lock_wait_time(returned_function->call_site, 
									               returned_lock_wait_time);

	TIME running_work = returned_function->running_work + 
					    returned_function->local_work;

	if (stacks->bottomFunctionIndex() == 0)
		return;

	std::shared_ptr<function_frame_t> parent_function = 
	 										     stacks->bottomParentFunction();
	// F.w += G.w
	parent_function->running_work += running_work;
	work->add_to_call_site(parent_function->call_site,
	                       parent_function->function_signature,
	                       running_work);
	parent_function->add_locks(returned_function);

	stacks->function_pop();
	print_event_end("RETURN");
}

void ParasiteTool::ThreadEnd(const ThreadEndEvent* e) {

	const ThreadEndInfo* _info(e->getThreadEndInfo());
	add_local_work(_info->endTime, "TE");
	std::shared_ptr<thread_frame_t> ending_thread(stacks->bottomThread());

	// G.p += G.c
	ending_thread->prefix.add(&(ending_thread->continuation));

	if (stacks->bottomThreadIndex() == 0)
		return;

	std::shared_ptr<thread_frame_t> parent_thread(stacks->bottomParentThread());
	parent_thread->join_vertex_list.push_back(ending_thread->last_vertex);
	parent_thread->concurrency_offset += _info->endTime - last_thread_start_time;
	print_time("concurrency_offset now", parent_thread->concurrency_offset);

	// if the ending thread is the longest child encountered so far
	// F is parent thread, G is ending thread
	if (ending_thread->prefix() + parent_thread->continuation() 
			                       > parent_thread->longest_child()) {

		std::cout << "ending thread is longest child encountered so far " << std::endl;

		// Save old bottom thread frame tables in 
		// parent frame's longest child variable.
		// F.l = G.p
		parent_thread->longest_child_lock_wait_time = ending_thread->lock_wait_time(); 
		parent_thread->longest_child.set(&(ending_thread->prefix));

		// F.p += F.c
		parent_thread->prefix.add(&(parent_thread->continuation));
	
		// F.c = 0
		parent_thread->continuation.clear();
	}

	parent_thread->add_child_locks(ending_thread);

	// pop the thread off the stack last, 
	// because the pop operation destroys the frame
	stacks->thread_pop();
	print_event_end("THREAD END");
}

void ParasiteTool::Acquire(const AcquireEvent* e) {

	print_event_start("ACQUIRE");
	const AcquireInfo* _info(e->getAcquireInfo());
	TIME acquire_time = _info->acquireTime;
	_info->lock->last_acquire_time = acquire_time;
	print_event_end("ACQUIRE");
}

void ParasiteTool::Release(const ReleaseEvent* e) {

	print_event_start("RELEASE");
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
	print_event_end("RELEASE");
}

void ParasiteTool::Access(const AccessEvent* e) {
	std::cout << "ERROR: Parasite Tool does not implement access event"
																   << std::endl;
	return;
}




