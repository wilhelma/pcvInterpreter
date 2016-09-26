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

#include <algorithm>    // std::max
#include <cassert>
#include <string>
#include <utility>
#include <limits>
#include <climits>

#include "AccessEvent.h"
#include "AcquireEvent.h"
#include "AcquireInfo.h"
#include "CallEvent.h"
#include "CallInfo.h"
#include "JoinEvent.h"
#include "JoinInfo.h"
#include "NewThreadEvent.h"
#include "NewThreadInfo.h"
#include "ParasiteTool.h"
#include "ReleaseEvent.h"
#include "ReleaseInfo.h"
#include "ReturnEvent.h"
#include "ReturnInfo.h"
#include "ThreadEndEvent.h"
#include "ThreadEndInfo.h"

ParasiteTool::ParasiteTool():thread_graph(random_string(5)), name(random_string(5)), 
							 jsonWriter(random_string(5)), last_event_time(static_cast<TIME>(0)) {
	thread_graph.name = name;
	jsonWriter.name = name;
}

vertex_descr_type ParasiteTool::add_edge(std::string end_vertex_label) {

	vertex_descr_type new_vertex = 
		thread_graph.add_edge(stacks.bottomThread()->last_vertex,
							  stacks.bottomThread()->current_edge_length,
							  end_vertex_label);
	stacks.bottomThread()->current_edge_length = static_cast<TIME>(0);
	stacks.bottomThread()->last_vertex = new_vertex;
	return new_vertex;
}

void ParasiteTool::add_join_edges(vertex_descr_type start, std::string label) {

	add_edge(label);
	thread_graph.add_join_edge(start, stacks.bottomThread()->last_vertex);
}

void ParasiteTool::add_down_stack(TIME local_work) {

	for (int i = 0; i <= stacks.bottomFunctionIndex(); i++) {
		work.add_to_call_site(stacks.functionAt(i)->call_site,
						  	   stacks.functionAt(i)->function_signature,
						  	   local_work);
		stacks.bottomThread()->continuation.add_to_call_site(
				 stacks.functionAt(i)->call_site, local_work);

	}
}

void ParasiteTool::add_start_time(CALLSITE call_site, TIME start_time) {
	std::pair<CALLSITE, TIME> newPair(call_site, start_time);
	start_time_hashtable.insert(newPair);
}

TIME ParasiteTool::concur(TIME serial_time) {
	return serial_time - stacks.bottomThread()->concurrency_offset;
}

void ParasiteTool::add_local_work(TIME strand_end_time, 
	  						                   std::string end_vertex_label) {

	print_event_start(end_vertex_label);
	print_time("strand_end_time", strand_end_time);
	print_time("last_event_time", last_event_time);
	assert(strand_end_time >= last_event_time);
	TIME local_work = strand_end_time - last_event_time;
	last_event_time = strand_end_time;
	work.add(local_work);
	stacks.bottomThread()->continuation.add(local_work);
	stacks.bottomThread()->current_edge_length += local_work;
	add_down_stack(local_work);
	print_time("local work", local_work);
}

void ParasiteTool::endProfileCalculations() {

	std::cout << "BOTTOM THREAD INDEX AT END IS " << stacks.bottomFunctionIndex() << std::endl;
	assert(stacks.bottomThreadIndex() == 0);
	std::cout << "BOTTOM FUNCTION INDEX AT END IS " << stacks.bottomFunctionIndex() << std::endl;
	assert(stacks.bottomFunctionIndex() == 0);

	std::shared_ptr<thread_frame_t> bottom_thread = stacks.bottomThread();
	std::shared_ptr<function_frame_t> bottom_function = stacks.bottomFunction();
	parasite_profile.lock_wait_time = bottom_function->lock_wait_time();

	// Calculate span for entire program 
	parasite_profile.span = bottom_thread->prefix();

	// Calculate work for entire program
	parasite_profile.work = work();

	// Calculate parallelism for entire program                     
	parasite_profile.parallelism =  static_cast<double> (parasite_profile.work)
				                   / static_cast<double> (parasite_profile.span);
}


void ParasiteTool::printOverallProfile() {

	std::cout << "PARALLELISM " << parasite_profile.parallelism << std::endl;
	print_time("WORK",  parasite_profile.work);
	print_time("SPAN",  parasite_profile.span);
	print_time("LOCK WAIT TIME",  parasite_profile.lock_wait_time);
	std::cout << "LOCK WAIT TIME IS " << 
						static_cast<double>(parasite_profile.lock_wait_time) /
						static_cast<double>(parasite_profile.span) <<
							                            " OF SPAN" << std::endl;
}

void ParasiteTool::outputOverallProfile() {

	if (COMMAND_LINE_OUTPUT)
		printOverallProfile();
	if (JSON_OUTPUT)
		jsonWriter.writeOverallProfile(parasite_profile);
}

void ParasiteTool::outputCallSites() {

	span.collect(&(stacks.bottomThread()->prefix));
	for (auto const &it : *work.hashtable) {
		CALLSITE key = it.first;
		if (span.hashtable->count(key)) {
			std::shared_ptr<CallSiteProfile> currentProfile
					(new CallSiteProfile(span.hashtable->at(key),
										 work.hashtable->at(key),
										 start_time_hashtable.at(key)));
			if (JSON_OUTPUT)
				jsonWriter.writeCallSite(currentProfile);
			if (COMMAND_LINE_OUTPUT)
				currentProfile->print();
		}
	}
}

ParasiteTool::~ParasiteTool() {

	std::cout << "Calling destructor" << std::endl;
	thread_graph.sink = thread_graph.last_vertex;
	endProfileCalculations();
	outputCallSites();
	outputOverallProfile();
	if (GRAPH_OUTPUT)
		thread_graph.write_dot_file();
}

void ParasiteTool::Call(const CallEvent* e) {

	const CallInfo* _info(e->getInfo());
	std::string call_label = "CALL_" + _info->fnSignature;
	add_local_work(_info->callTime, call_label);
    stacks.bottomThread()->continuation.init_call_site(_info->siteId);
    work.record_call_site(_info->siteId, _info->fnSignature);
    add_start_time(_info->siteId, concur(_info->callTime));
	stacks.function_push(_info->fnSignature, _info->siteId);
	print_event_end("CALL");
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {

	const NewThreadInfo* const _info = e->getInfo();
	vertex_descr_type thread_start_vertex;
	if (stacks.bottomThreadIndex() != -1) {
		stacks.bottomThread()->spawned_children_count += 1;
		std::string new_thread_label = "TS_" + std::to_string(static_cast<unsigned>(_info->childThread->threadId));
		add_local_work(_info->startTime, new_thread_label);
		thread_start_vertex = add_edge(new_thread_label);
	} else {
		thread_start_vertex = thread_graph.last_vertex;
		last_event_time = _info->startTime;
	}

	stacks.thread_push(stacks.bottomFunctionIndex(),     
		                _info->childThread->threadId, 
		                thread_start_vertex);
	if (stacks.bottomThreadIndex() != 0)
		stacks.bottomThread()->concurrency_offset += stacks.bottomParentThread()->concurrency_offset;
	print_event_end("NEW THREAD");
}

void ParasiteTool::Join(const JoinEvent* e) {

	const JoinInfo* const _info = e->getInfo();
	print_event_start("JOIN");
	std::shared_ptr<thread_frame_t> bottom_thread = stacks.bottomThread();
	bottom_thread->spawned_children_count -= 1;
	std::string join_label = "JOIN_" + std::to_string(static_cast<unsigned>(_info->childThread->threadId)) + "_" + 
								  std::to_string(static_cast<unsigned>(_info->parentThread->threadId));
	
	add_local_work(_info->joinTime, join_label);				  
	add_join_edges(bottom_thread->join_vertex_list.front(), join_label);
	if (bottom_thread->join_vertex_list.size())
		bottom_thread->join_vertex_list.pop_front();

	if (bottom_thread->spawned_children_count == 0) {
	 
		print_event_start("SYNC");

		// If critical path goes through spawned child
		if (bottom_thread->longest_child() > bottom_thread->continuation())
			bottom_thread->prefix.add(&(bottom_thread->longest_child));
		else 
			bottom_thread->prefix.add(&(bottom_thread->continuation));

		bottom_thread->continuation.clear();
		bottom_thread->longest_child.clear();
		print_event_end("SYNC");
	}
	print_event_end("JOIN");
}

// Called G returns to F
void ParasiteTool::Return(const ReturnEvent* e) {

	const ReturnInfo* _info(e->getInfo());
	FUN_SG fun_sg = stacks.bottomFunction()->function_signature;
	std::string return_label = "R_" + std::to_string(static_cast<unsigned>(_info->call)) + "_" + fun_sg;
	add_local_work(_info->endTime, return_label);
	if (DEBUG_OUTPUT) {
		std::cout << "starting return Event with signature " <<
			stacks.bottomFunction()->function_signature.c_str() << std::endl;
	}

	std::shared_ptr<function_frame_t> returned_function(stacks.bottomFunction());
	stacks.bottomThread()->continuation.
						    add_lock_wait_time(returned_function->call_site, 
									           returned_function->lock_wait_time());
	if (stacks.bottomFunctionIndex() == 0) 
		return;
								
	stacks.bottomParentFunction()->add_locks(returned_function);
	stacks.function_pop();
	print_event_end("RETURN");
}

void ParasiteTool::ThreadEnd(const ThreadEndEvent* e) {

	const ThreadEndInfo* _info(e->getInfo());
	std::string thread_end_label = "TE_" + std::to_string(static_cast<unsigned>(_info->id));
	add_local_work(_info->endTime, thread_end_label);
	add_edge(thread_end_label);
	std::shared_ptr<thread_frame_t> ending_thread(stacks.bottomThread());
	ending_thread->prefix.add(&(ending_thread->continuation));
	span.collect(&(ending_thread->prefix));

	if (stacks.bottomThreadIndex() == 0) {
		return;
	}

	std::shared_ptr<thread_frame_t> parent_thread(stacks.bottomParentThread());
	parent_thread->join_vertex_list.push_back(ending_thread->last_vertex);
	parent_thread->concurrency_offset += ending_thread->prefix();

	// if the ending thread is the longest child encountered so far
	if (ending_thread->prefix() + parent_thread->continuation() 
			                       > parent_thread->longest_child()) {

		if (DEBUG_OUTPUT) {
			std::cout << "ending thread is longest child encountered so far " << std::endl;
		}
		parent_thread->longest_child.set(&(ending_thread->prefix));
		parent_thread->prefix.add(&(parent_thread->continuation));
		parent_thread->continuation.clear();
	}

	parent_thread->add_child_locks(ending_thread);
	stacks.thread_pop();
	print_event_end("THREAD END");
}

void ParasiteTool::Acquire(const AcquireEvent* e) {

	print_event_start("ACQUIRE");
	const AcquireInfo* _info(e->getInfo());
	TIME acquire_time = _info->acquireTime;
	_info->lock->last_acquire_time = acquire_time;
	print_event_end("ACQUIRE");
}

void ParasiteTool::Release(const ReleaseEvent* e) {

	print_event_start("RELEASE");
	const ReleaseInfo* _info(e->getInfo());
	TIME release_time = _info->releaseTime;
	assert(static_cast<double>(release_time) >
		   static_cast<double>(_info->lock->last_acquire_time));
	
	unsigned int lockId = _info->lock->lockId;

	stacks.bottomThread()->
		lock_intervals.addInterval(
					 concur(_info->lock->last_acquire_time), 
					 concur(release_time), lockId);
	stacks.bottomFunction()->
		lock_intervals.addInterval(
					 concur(_info->lock->last_acquire_time), 
					 concur(release_time), lockId);
	print_event_end("RELEASE");
}

void ParasiteTool::Access(const AccessEvent* e) {}
