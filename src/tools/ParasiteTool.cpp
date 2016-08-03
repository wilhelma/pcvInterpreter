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
#include <utility>
#include <limits>
#include <climits>
#include "ParasiteTool.h"

ParasiteTool::ParasiteTool():last_function_call_time(0), 
							 last_function_return_time(0),
							 last_function_runtime(0), 
							 last_thread_end_time(0), 
							 last_thread_runtime(0),
							 last_thread_start_time(0),
							 lock_span_end_time(0),
							 lock_span_start_time(0),
							 last_event_time(0),
							 last_thread_event_time(0) {

	stacks = std::unique_ptr<ParasiteTracker>(new ParasiteTracker());
	parasite_profile = std::unique_ptr<parasite_profile_t> (new parasite_profile_t);
	std::shared_ptr<CallSiteWorkHashtable> wrk_table(new CallSiteWorkHashtable());
	work_table = wrk_table;
	std::unordered_map<unsigned int, int> lck_hashtable;
	lock_hashtable = lck_hashtable;
}

vertex_descr_type ParasiteTool::add_edge(TIME length, std::string end_vertex_label) {

	vertex_descr_type new_vertex = 
		thread_graph.add_edge(stacks->bottomThread()->last_vertex, length,
							  end_vertex_label);
	stacks->bottomThread()->last_vertex = new_vertex;
	return new_vertex;
}

void ParasiteTool::add_join_edge(vertex_descr_type start) {

	thread_graph.add_join_edge(start, stacks->bottomThread()->last_vertex);
}


void ParasiteTool::printOverallProfile() {
	assert(stacks->bottomThreadIndex() == 0);
	assert(stacks->bottomFunctionIndex() == 0);
	std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();
	std::shared_ptr<function_frame_t> bottom_function_frame = stacks->bottomFunction();

	// Calculate span for entire program 
	parasite_profile->span =  static_cast<TIME> (bottom_thread_frame->prefix_span +
												 bottom_thread_frame->continuation_span);
	printf("bottom thread prefix span is %llu \n", (unsigned long long) bottom_thread_frame->prefix_span);
	printf("bottom thread local continuation is %llu \n", (unsigned long long) bottom_thread_frame->continuation_span);

	// Calculate work for entire program
	parasite_profile->work = static_cast<TIME>(bottom_function_frame->running_work + bottom_function_frame->local_work);
	printf("bottom function local work is %llu \n", (unsigned long long) bottom_function_frame->local_work);
	printf("bottom function running work is %llu \n", (unsigned long long) bottom_function_frame->running_work);

	// Calculate parallelism for entire program                     
	parasite_profile->parallelism =  static_cast<double> (parasite_profile->work)
				                   / static_cast<double> (parasite_profile->span);

	printf("PARALLELISM IS %f \n", parasite_profile->parallelism);
	printf("WORK IS %llu \n", (unsigned long long) parasite_profile->work);
	printf("SPAN IS %llu \n", (unsigned long long) parasite_profile->span);
	printf("LOCK SPAN IS %llu \n", (unsigned long long) parasite_profile->lock_span);
}

void ParasiteTool::printCallSiteProfiles() {

	std::shared_ptr<thread_frame_t> bottom_thread_frame = 
														 stacks->bottomThread();

	// Print out information for each call site
    CallSiteSpanHashtable bottom_prefix_table(bottom_thread_frame->prefix_table);
	bottom_prefix_table.add(&(bottom_thread_frame->continuation_table));
	std::shared_ptr<call_site_work_hashtable_t> wrk_table = work_table->hashtable;

	for (auto const &it : *wrk_table) {
		CALLSITE key = it.first;
		CallSiteProfile currentProfile(bottom_prefix_table.hashtable->at(key), wrk_table->at(key));
		currentProfile.print();
	}
}

void ParasiteTool::printGraphInformation() {

	std::vector<double> path = thread_graph.shortest_paths(thread_graph.source);
	for (std::vector<double>::const_iterator i = path.begin(); i != path.end(); ++i)
    	std::cout << *i << std::endl;
}

void ParasiteTool::printProfile() {

	printCallSiteProfiles();
	printOverallProfile();
	printGraphInformation();
}

void ParasiteTool::writeJson() {

	std::shared_ptr<ParasiteJsonWriter> parasiteJsonWriter(new ParasiteJsonWriter());
	std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();

	// Output json information for whole profile
	parasiteJsonWriter->writeOverallProfile(parasite_profile);

	// Output json information for each call site
    CallSiteSpanHashtable bottom_prefix_table(bottom_thread_frame->prefix_table);
	bottom_prefix_table.add(&(bottom_thread_frame->continuation_table));
	std::shared_ptr<call_site_work_hashtable_t> wrk_table = work_table->hashtable;

	for (auto const &it : *wrk_table) {
		CALLSITE key = it.first;
		std::shared_ptr<CallSiteProfile> currentProfile
					(new CallSiteProfile(bottom_prefix_table.hashtable->at(key),
									     wrk_table->at(key)));
		parasiteJsonWriter->writeCallSite(currentProfile);
	}

}

ParasiteTool::~ParasiteTool() {
	printf("Calling destructor \n");

	if (COMMAND_LINE_OUTPUT) {
		printf("================ \n");
		printf("PRINT BEGIN \n");
		printProfile();
		printf("PRINT END \n");
		printf("================ \n");
	}

	if (JSON_OUTPUT) 
		writeJson();

	if (GRAPH_OUTPUT)
		thread_graph.write_dot_file();
}

void ParasiteTool::Call(const CallEvent* e) {

	printf("================ \n");
	printf("CALL BEGIN \n");
	const CallInfo* _info(e->getCallInfo());
	FUN_SG calledFunctionSignature = _info->fnSignature;
	CALLSITE callsiteID = _info->siteId;
	last_function_runtime = _info->runtime;
	last_function_call_time = _info->callTime;
	last_event_time = _info->callTime;
	printf("starting call Event with signature %s \n", calledFunctionSignature.c_str());

	work_table->increment_count(callsiteID, calledFunctionSignature);
	stacks->function_push(calledFunctionSignature, callsiteID);
	printf("CALL END \n");
	printf("================ \n");
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {
	printf("================ \n");
	printf("NEW THREAD BEGIN \n");
	const NewThreadInfo* const _info = e->getNewThreadInfo();
	const TRD_ID newThreadID = _info->childThread->threadId;

	// get information about the thread's head function
	TIME create_time = _info->startTime;
	TIME last_thread_event_time = _info->startTime;
	last_thread_start_time = _info->startTime;

	if (stacks->bottomThreadIndex() != -1) {
		std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();
		TIME local_work = static_cast<TIME>(create_time - last_event_time);
		vertex_descr_type thread_start_vertex = add_edge(local_work, "TS");
		printf("using local work of %llu in new thread event \n", static_cast<unsigned long long>(local_work));
		assert(local_work >= 0);
		bottom_thread_frame->continuation_span += local_work;
		bottom_thread_frame->continuation_table.add_span(stacks->bottomFunction()->call_site, local_work);
		std::shared_ptr<function_frame_t> bottom_function_frame = stacks->bottomFunction();
		bottom_function_frame->local_work += local_work;
		work_table->add_work(bottom_function_frame->call_site, bottom_function_frame->function_signature, local_work);

		last_event_time = create_time;
		std::shared_ptr<thread_frame_t> new_thread_frame = 
				stacks->thread_push(stacks->bottomFunctionIndex(), newThreadID,
									thread_start_vertex);
	}

	else {

		vertex_descr_type thread_start_vertex = thread_graph.last_vertex;
		last_event_time = create_time;
		std::shared_ptr<thread_frame_t> new_thread_frame = 
				stacks->thread_push(stacks->bottomFunctionIndex(), newThreadID,
									thread_start_vertex);
	}

	printf("NEW THREAD END \n");
	printf("================ \n");
}

void ParasiteTool::Join(const JoinEvent* e) {
	 
	printf("================ \n");
	printf("STARTING JOIN \n");
	std::shared_ptr<thread_frame_t> bottom_thread_frame(stacks->bottomThread());
	// bottom_function_frame->running_lock_span += bottom_thread_frame->local_lock_span;
	printf("IN JOIN: longest child span is %llu \n", (unsigned long long) bottom_thread_frame->longest_child_span);
	printf("IN JOIN: local continuation is %llu \n", (unsigned long long) bottom_thread_frame->continuation_span);


	add_join_edge(bottom_thread_frame->join_vertex_list.front());
	bottom_thread_frame->join_vertex_list.pop_front();

	 // If critical path goes through spawned child

	// If F.l > F.c
	if (bottom_thread_frame->longest_child_span > bottom_thread_frame->continuation_span) {

		// F.p += F.l
		bottom_thread_frame->prefix_span += bottom_thread_frame->longest_child_span;
		bottom_thread_frame->lock_span += lock_span_end_time - lock_span_start_time;

		assert(bottom_thread_frame->lock_span == static_cast<TIME>(0));
		assert(bottom_thread_frame->longest_child_lock_span == static_cast<TIME>(0));
		// bottom_thread_frame->prefix_span += bottom_thread_frame->lock_span;
		// bottom_thread_frame->prefix_span -= bottom_thread_frame->
		                                    // longest_child_lock_span;
		CallSiteSpanHashtable prefix_table(bottom_thread_frame->prefix_table);
		prefix_table.add(&(bottom_thread_frame->longest_child_table));
		//local_span does not increase, because critical path goes 
		//through spawned child.
	} // F.c > F.l
		else {

		// F.p += F.c 
		bottom_thread_frame->prefix_span += bottom_thread_frame->continuation_span;
		// Critical path goes through continuation, which is local. Add
		// continuation_span to local_span.
		CallSiteSpanHashtable prefix_table(bottom_thread_frame->prefix_table);
		prefix_table.add(&(bottom_thread_frame->continuation_table));
	}

	// reset longest child and continuation span variables
	// F.l = 0
	bottom_thread_frame->longest_child_span = static_cast<TIME>(0);
	bottom_thread_frame->longest_child_lock_span = static_cast<TIME>(0);
	// F.c = 0
	// bottom_thread_frame->continuation_span = static_cast<TIME>(0);
	printf("JOIN END \n");
	printf("================ \n");
}

// Called G returns to F
void ParasiteTool::Return(const ReturnEvent* e) {
	printf("================ \n");
	printf("RETURN START \n");
	const ReturnInfo* _info(e->getReturnInfo());
	TIME returnTime = _info->endTime;
	assert(returnTime >= last_event_time);
	TIME local_work = static_cast<TIME> (returnTime - last_event_time);
	add_edge(local_work, "R");
	last_event_time = returnTime;
	printf("last event time is now %llu \n", (unsigned long long) last_event_time);
	last_function_return_time = returnTime;
	printf("performing return operations for local work %llu \n", (unsigned long long) local_work);

	std::shared_ptr<function_frame_t> returned_function_frame(stacks->bottomFunction());
	CALLSITE returning_call_site = returned_function_frame->call_site;
	returned_function_frame->local_work += local_work;

	std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();
	bottom_thread_frame->prefix_span += local_work;
	work_table->add_work(returning_call_site, returned_function_frame->function_signature, local_work);

	TIME running_work = static_cast<TIME>(returned_function_frame->running_work + local_work);
	//TIME running_lock_span = static_cast<TIME>(returned_function_frame->running_lock_span + 
																						 // returned_function_frame->local_lock_span)

	CallSiteSpanHashtable bottom_thread_continuation_table(stacks->
														   bottomThread()->
														   continuation_table);


	if (stacks->bottomFunctionIndex() == 0) {
		bottom_thread_continuation_table.add_span(returning_call_site, local_work);
		return;
	}

	// F.c += G.p
	bottom_thread_continuation_table.add_span(returning_call_site, running_work);

	std::shared_ptr<function_frame_t> parent_function_frame = stacks->bottomParentFunction();
	// F.w += G.w
	parent_function_frame->running_work += running_work;
	// parent_function_frame->running_lock_span += running_lock_span;
	work_table->add_work(parent_function_frame->call_site, parent_function_frame->function_signature, running_work);

	stacks->function_pop();
	printf("RETURN END \n");
	printf("================ \n");
}

void ParasiteTool::ThreadEnd(const ThreadEndEvent* e) {
	printf("================ \n");
	printf("THREAD END BEGIN \n");
	const ThreadEndInfo* _info(e->getThreadEndInfo());
	last_thread_end_time = _info->endTime;
	TIME last_thread_event_time = _info->endTime;
	TIME this_thread_start_time = stacks->bottomThread()->thread_start_time;
	TIME last_thread_runtime = static_cast<TIME>(_info->endTime - this_thread_start_time);
	TIME local_work = static_cast<TIME>(_info->endTime - last_event_time);
	add_edge(local_work, "TE");
	printf("local work in thread end is %llu \n", (unsigned long long) local_work);
	last_event_time = _info->endTime;
	printf("last_event_time now %llu \n", (unsigned long long) last_event_time);

	std::shared_ptr<function_frame_t> current_function_frame = stacks->bottomFunction();
	current_function_frame->local_work += local_work;
	work_table->add_work(current_function_frame->call_site,
						 current_function_frame->function_signature, 
						 local_work);
	stacks->bottomThread()->continuation_span += local_work;
	stacks->bottomThread()->continuation_table.add_span(stacks->bottomFunction()->call_site, local_work);

	if (stacks->bottomThreadIndex() == 0)
		return;

	stacks->bottomParentThread()->
			   join_vertex_list.push_back(stacks->bottomThread()->last_vertex);

	std::shared_ptr<thread_frame_t> ending_thread_frame(stacks->bottomThread());
	std::shared_ptr<thread_frame_t> parent_thread_frame(stacks->bottomParentThread());
	CallSiteSpanHashtable parent_thread_prefix_table(stacks->bottomThread()->prefix_table);
	parent_thread_prefix_table.add_span(current_function_frame->call_site, 
										ending_thread_frame->prefix_span);

	// if the ending thread is the longest child encountered so far
	// F is parent thread, G is ending thread
	if (ending_thread_frame->prefix_span + parent_thread_frame->continuation_span 
																			 > parent_thread_frame->longest_child_span) {

		printf("ending thread is longest child encountered so far \n");

		// Save old bottom thread frame tables in 
		// parent frame's longest child variable.
		// F.l = G.p
		ending_thread_frame->prefix_span += local_work;
		parent_thread_frame->longest_child_span = ending_thread_frame->prefix_span;
		printf("longest child span of frame %d is now %llu \n",
						stacks->bottomThreadIndex() - 1, 
						(unsigned long long) ending_thread_frame->prefix_span);  
		parent_thread_frame->longest_child_table.clear();
		ending_thread_frame->prefix_table.add(&(ending_thread_frame->continuation_table));
		parent_thread_frame->longest_child_table.add(&(ending_thread_frame->prefix_table));

		// F.p += F.c
		CallSiteSpanHashtable prefix_table(parent_thread_frame->prefix_table);
		prefix_table.add(&(parent_thread_frame->continuation_table));
		parent_thread_frame->prefix_span += parent_thread_frame->continuation_span;

		// F.c = 0
		parent_thread_frame->continuation_table.clear();
		parent_thread_frame->continuation_span = static_cast<TIME>(0);
	}

	// pop the thread off the stack last, 
	// because the pop operation destroys the frame
	stacks->thread_pop();
	printf("THREAD END END \n");
	printf("================ \n");
}

void ParasiteTool::Acquire(const AcquireEvent* e) {

	const AcquireInfo* _info(e->getAcquireInfo());
	std::shared_ptr<ShadowLock> acquiredLock(_info->lock);

	// TIME acquireTime = e->acquireTime;
	// acquiredLock->last_acquire_time = e->acquireTime;

	TIME acquire_time = static_cast<TIME>(0);

	if ((acquire_time - last_thread_runtime) < lock_span_start_time)
		lock_span_start_time = acquire_time;
	else 
		lock_span_start_time += last_thread_runtime;

	// acquiredLock->last_acquire_time = static_cast<TIME>0;
	lock_span_start_time = static_cast<TIME>(0);

	unsigned int lockId = acquiredLock->lockId;

	if (lock_hashtable.count(lockId)) {
		lock_hashtable.at(lockId) = stacks->bottomFunctionIndex();
	} else {
		std::pair<unsigned int, int> newPair(lockId, stacks->bottomFunctionIndex());
		lock_hashtable.insert(newPair);
	}

}

void ParasiteTool::Release(const ReleaseEvent* e) {

	const ReleaseInfo* _info(e->getReleaseInfo());
	std::shared_ptr<ShadowLock> releasedLock(_info->lock);
	// release_time = e->releaseTime;

	TIME release_time = static_cast<TIME>(0);
	// TIME lock_span = static_cast<TIME>(0);

	// TIME lock_span = release_time - releasedLock->last_acquire_time;

	// unsigned int lockId = releasedLock->lockId;
	unsigned int lockId = (unsigned int) 0;

	int unlocked_function_index = lock_hashtable.at(lockId);
	std::shared_ptr<function_frame_t> unlocked_function_frame
								(stacks->functionAt(unlocked_function_index));
	// unlocked_function_frame->local_lock_span += lock_span;

	if ((release_time - last_thread_runtime) > lock_span_end_time)
		lock_span_start_time = release_time;
	else 
		lock_span_start_time += last_thread_runtime;
}

void ParasiteTool::Access(const AccessEvent* e) {
	printf("ERROR: Parasite Tool does not implement access event");
	return;
}




