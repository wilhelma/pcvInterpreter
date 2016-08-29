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

ParasiteTool::ParasiteTool():thread_graph(random_string(5)), name(random_string(5)), 
							 last_event_time(0) {
	thread_graph.name = name;
}

vertex_descr_type ParasiteTool::add_edge(TIME length, std::string end_vertex_label) {

	vertex_descr_type new_vertex = 
		thread_graph.add_edge(stacks.bottomThread()->last_vertex, length,
							  end_vertex_label);
	stacks.bottomThread()->last_vertex = new_vertex;
	return new_vertex;
}

void ParasiteTool::add_join_edges(vertex_descr_type start) {

	add_edge(0, "J");
	thread_graph.add_join_edge(start, stacks.bottomThread()->last_vertex);
}

void ParasiteTool::add_down_stack(TIME local_work, TIME parallel_time) {

	for (int i = 0; i <= stacks.bottomFunctionIndex(); i++) {
		work.add_to_call_site(stacks.functionAt(i)->call_site,
						  	   stacks.functionAt(i)->function_signature,
						  	   local_work);
		stacks.bottomThread()->continuation.add_to_call_site(
				 stacks.functionAt(i)->call_site, local_work, parallel_time);
	}
}

TIME ParasiteTool::concur(TIME serial_time) {
	return serial_time - stacks.bottomThread()->concurrency_offset;
}

vertex_descr_type ParasiteTool::add_local_work(TIME strand_end_time, 
	  						                   std::string end_vertex_label) {

	print_event_start(end_vertex_label);
	assert(strand_end_time >= last_event_time);
	TIME local_work = strand_end_time - last_event_time;
	last_event_time = strand_end_time;
	work.add(local_work);
	stacks.bottomThread()->continuation.add(local_work);
	add_down_stack(local_work, concur(strand_end_time));
	print_time("local work", local_work);
	print_time("last_event_time", last_event_time);
	return add_edge(local_work, end_vertex_label);
}

void ParasiteTool::endProfileCalculations() {

	assert(stacks.bottomThreadIndex() == 0);
	assert(stacks.bottomFunctionIndex() == 0);
	std::shared_ptr<thread_frame_t> bottom_thread = stacks.bottomThread();
	std::shared_ptr<function_frame_t> bottom_function = stacks.bottomFunction();
	parasite_profile.lock_wait_time = bottom_function->lock_wait_time();

	// Calculate span for entire program 
	parasite_profile.span = bottom_thread->prefix();

	// Calculate work for entire program
	parasite_profile.work = work() + parasite_profile.lock_wait_time;

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

void ParasiteTool::printCallSiteProfiles() {

	// bottom prefix table contains spans for all call sites at program end
    Span bottom_prefix(stacks.bottomThread()->prefix);
	std::shared_ptr<call_site_work_hashtable_t> wrk = work.hashtable;
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

	std::shared_ptr<ParasiteJsonWriter> parasiteJsonWriter(new ParasiteJsonWriter(name));
	std::shared_ptr<thread_frame_t> bottom_thread = stacks.bottomThread();

	// Output json information for whole profile
	parasiteJsonWriter->writeOverallProfile(parasite_profile);

	// Output json information for each call site
    Span bottom_prefix(bottom_thread->prefix);
	std::shared_ptr<call_site_work_hashtable_t> wrk = work.hashtable;

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
	last_event_time = _info->callTime;
	std::cout << "starting call Event with signature " <<
				      _info->fnSignature.c_str() << std::endl;
    stacks.bottomThread()->continuation.init_call_site(_info->siteId, 
    												   concur(_info->callTime));
    work.record_call_site(_info->siteId, _info->fnSignature);
	stacks.function_push(_info->fnSignature, _info->siteId);
	print_event_end("CALL");
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {

	const NewThreadInfo* const _info = e->getNewThreadInfo();
	vertex_descr_type thread_start_vertex;

	if (stacks.bottomThreadIndex() != -1) 
		thread_start_vertex = add_local_work(_info->startTime, "TS");
	else 
		thread_start_vertex = thread_graph.last_vertex;

	stacks.thread_push(stacks.bottomFunctionIndex(),     
		                _info->childThread->threadId, 
		                thread_start_vertex,
		                _info->startTime);

	print_event_end("NEW THREAD");
}

void ParasiteTool::Join(const JoinEvent* e) {
	 
	print_event_start("JOIN");
	std::shared_ptr<thread_frame_t> bottom_thread(stacks.bottomThread());
	add_join_edges(bottom_thread->join_vertex_list.front());
	bottom_thread->join_vertex_list.pop_front();

	TIME lock_wait_time_excluding_children = bottom_thread->lock_wait_time();
	bottom_thread->absorb_child_locks();
	TIME lock_wait_time_including_children = bottom_thread->lock_wait_time();
	TIME lock_wait_time_on_continuation = lock_wait_time_including_children - 
								   		  lock_wait_time_excluding_children;

	// If critical path goes through spawned child
	if (bottom_thread->longest_child() > bottom_thread->continuation()) {
		bottom_thread->prefix.add(&(bottom_thread->longest_child));
		// avoid double counting of lock wait time
		bottom_thread->
					correct_prefix(bottom_thread->longest_child_lock_wait_time);
	} else { 
		bottom_thread->prefix.add(&(bottom_thread->continuation));
		// avoid double counting of lock wait time
		bottom_thread->correct_prefix(lock_wait_time_on_continuation);
	}

	bottom_thread->continuation.clear();
	bottom_thread->longest_child.clear();
	print_event_end("JOIN");
}

// Called G returns to F
void ParasiteTool::Return(const ReturnEvent* e) {

	const ReturnInfo* _info(e->getReturnInfo());
	add_local_work(_info->endTime, "R");
	std::cout << "starting return Event with signature " <<
		stacks.bottomFunction()->function_signature.c_str() << std::endl;

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

	const ThreadEndInfo* _info(e->getThreadEndInfo());
	add_local_work(_info->endTime, "TE");
	std::shared_ptr<thread_frame_t> ending_thread(stacks.bottomThread());
	ending_thread->continuation.add(ending_thread->lock_wait_time());
	ending_thread->prefix.add(&(ending_thread->continuation));

	if (stacks.bottomThreadIndex() == 0) {
		return;
	}

	std::shared_ptr<thread_frame_t> parent_thread(stacks.bottomParentThread());
	parent_thread->join_vertex_list.push_back(ending_thread->last_vertex);
	parent_thread->concurrency_offset += _info->endTime - ending_thread->start_time;

	// if the ending thread is the longest child encountered so far
	if (ending_thread->prefix() + parent_thread->continuation() 
			                       > parent_thread->longest_child()) {

		std::cout << "ending thread is longest child encountered so far " << std::endl;
		parent_thread->longest_child_lock_wait_time = ending_thread->lock_wait_time(); 
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
