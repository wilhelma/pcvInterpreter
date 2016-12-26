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

ParasiteTool::ParasiteTool():name(random_string(5)), 
							 jsonWriter(random_string(5)), last_event_time(static_cast<TIME>(0)),
							 concurrency_offset(static_cast<TIME>(0)) {
	jsonWriter.name = name;
}

void ParasiteTool::add_down_stack(TIME local_work) {

	for (int i = 0; i <= stacks.bottomFunctionIndex(); i++) {

		if (stacks.functionAt(i)->topCall) {
			work.add_to_call_site(stacks.functionAt(i)->call_site,
						  	   stacks.functionAt(i)->function_signature,
						  	   local_work);
		}
	
		if (stacks.functionAt(i)->topCallOnThread) {
			stacks.bottomThread()->continuation.add_to_call_site(
			   					stacks.functionAt(i)->call_site, local_work);
		}
	}
}

void ParasiteTool::add_lock_wait_time_down_stack(TIME wait_time) {

	for (int i = 0; i <= stacks.bottomFunctionIndex(); i++) {
	
		if (stacks.functionAt(i)->topCallOnThread) {

			stacks.bottomThread()->continuation.add_lock_wait_time(
				stacks.functionAt(i)->call_site, wait_time);
		}
	}
}

void ParasiteTool::add_start_time(CALLSITE call_site, TIME start_time) {
	std::pair<CALLSITE, TIME> newPair(call_site, start_time);
	start_time_hashtable.insert(newPair);
}

TIME ParasiteTool::concur(TIME serial_time) {
	return serial_time - concurrency_offset;
}

void ParasiteTool::add_local_work(TIME strand_end_time) {

	assert(strand_end_time >= last_event_time);
	TIME local_work = strand_end_time - last_event_time;
	last_event_time = strand_end_time;
	work.add(local_work);
	stacks.bottomThread()->continuation.add(local_work);
	add_down_stack(local_work);
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
	parasite_profile.work = work();

	// Calculate parallelism for entire program                     
	parasite_profile.parallelism =  static_cast<double> (parasite_profile.work)
				                   / static_cast<double> (parasite_profile.span);

 	TIME min = std::numeric_limits<TIME>::max();
 	for (auto &it : start_time_hashtable) {
    	if (it.second < min)
    		min = it.second;
    }

    for (auto &it : start_time_hashtable) {
    	it.second -= min;
    }
}


void ParasiteTool::outputOverallProfile() {
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

			jsonWriter.writeCallSite(currentProfile);
		}
	}
}

ParasiteTool::~ParasiteTool() {

	endProfileCalculations();
	outputCallSites();
	outputOverallProfile();
}

void ParasiteTool::Call(const CallEvent* e) {

	const CallInfo* _info(e->getInfo());
	std::string call_label = "CALL_" + std::to_string(static_cast<unsigned>(_info->siteId)) + "_" +
																	     _info->fnSignature;
	if (stacks.bottomFunctionIndex() > -1)	
		add_local_work(_info->callTime);
	else
		last_event_time = _info->callTime;
	
    stacks.bottomThread()->continuation.init_call_site(_info->siteId);
    work.record_call_site(_info->siteId, _info->fnSignature);
    add_start_time(_info->siteId, concur(_info->callTime));

	int topCall = !std::count(already_called_list.begin(), 
							  already_called_list.end(),
						      _info->siteId);
	if (topCall) {
    	already_called_list.push_back(_info->siteId);
	}

	int topCallOnThread = !std::count(stacks.bottomThread()->already_called_list.begin(), 
									 stacks.bottomThread()->already_called_list.end(),
									 _info->siteId);
	if (topCallOnThread) {
    	stacks.bottomThread()->already_called_list.push_back(_info->siteId);
	}

	stacks.function_push(_info->fnSignature, _info->siteId, topCall, topCallOnThread);
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {

	const NewThreadInfo* const _info = e->getInfo();
	if (stacks.bottomThreadIndex() != -1) {
		stacks.bottomThread()->spawned_children_count += 1;
		std::string new_thread_label = "TS_" + std::to_string(static_cast<unsigned>(_info->childThread->threadId));
		add_local_work(_info->startTime);
	} else {
		last_event_time = _info->startTime;
	}

	stacks.thread_push(stacks.bottomFunctionIndex(),     
		                _info->childThread->threadId);
}

void ParasiteTool::Join(const JoinEvent* e) {

	const JoinInfo* const _info = e->getInfo();
	std::shared_ptr<thread_frame_t> bottom_thread = stacks.bottomThread();
	bottom_thread->spawned_children_count -= 1;
	std::string join_label = "JOIN_" + std::to_string(static_cast<unsigned>(_info->childThread->threadId)) + "_" + 
								  std::to_string(static_cast<unsigned>(_info->parentThread->threadId));
	
	add_local_work(_info->joinTime);				  

	if (bottom_thread->spawned_children_count == 0) {

		// If critical path goes through spawned child
		if (bottom_thread->longest_child() > bottom_thread->continuation())
			bottom_thread->prefix.add(&(bottom_thread->longest_child));
		else 
			bottom_thread->prefix.add(&(bottom_thread->continuation));

		bottom_thread->continuation.clear();
		bottom_thread->longest_child.clear();
	}
}

// Called G returns to F
void ParasiteTool::Return(const ReturnEvent* e) {

	const ReturnInfo* _info(e->getInfo());
	FUN_SG fun_sg = stacks.bottomFunction()->function_signature;
	std::shared_ptr<function_frame_t> returned_function(stacks.bottomFunction());
	std::string return_label = "R_"
								+ std::to_string(static_cast<unsigned>(returned_function->call_site))
							    + "_" + fun_sg;
	add_local_work(_info->endTime);
	add_lock_wait_time_down_stack(returned_function->lock_wait_time());
	if (stacks.bottomFunctionIndex() == 0) 
		return;
								
	stacks.bottomParentFunction()->add_locks(returned_function);

	if (stacks.bottomFunction()->topCall) {
		std::vector<CALLSITE>::iterator position = std::find(already_called_list.begin(), 
															 already_called_list.end(),
															 returned_function->call_site);
		already_called_list.erase(position);
	}

	if (stacks.bottomFunction()->topCallOnThread) {
		std::vector<CALLSITE>::iterator position = std::find(stacks.bottomThread()->already_called_list.begin(), 
															 stacks.bottomThread()->already_called_list.end(),
															 returned_function->call_site);
		stacks.bottomThread()->already_called_list.erase(position);
	}

	stacks.function_pop();
}

void ParasiteTool::ThreadEnd(const ThreadEndEvent* e) {

	const ThreadEndInfo* _info(e->getInfo());
	std::string thread_end_label = "TE_" + std::to_string(static_cast<unsigned>(_info->id));
	add_local_work(_info->endTime); 
	std::shared_ptr<thread_frame_t> ending_thread(stacks.bottomThread());
	ending_thread->prefix.add(&(ending_thread->continuation));

	if (stacks.bottomThreadIndex() == 0) {
		return;
	}

	std::shared_ptr<thread_frame_t> parent_thread(stacks.bottomParentThread());
	concurrency_offset += ending_thread->prefix();

	// if the ending thread is the longest child encountered so far
	if (ending_thread->prefix() + parent_thread->continuation() 
			                       > parent_thread->longest_child()) {

		parent_thread->longest_child.set(&(ending_thread->prefix));
		parent_thread->prefix.add(&(parent_thread->continuation));
		parent_thread->continuation.clear();
	}


	parent_thread->add_child_locks(ending_thread);
	stacks.thread_pop();
}

void ParasiteTool::Acquire(const AcquireEvent* e) {

	const AcquireInfo* _info(e->getInfo());
	TIME acquire_time = _info->acquireTime;
	_info->lock->last_acquire_time = acquire_time;
}

void ParasiteTool::Release(const ReleaseEvent* e) {

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
}

void ParasiteTool::Access(const AccessEvent* e) {}
