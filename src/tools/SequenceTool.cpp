/**
 *
 *    @file  SequenceTool.cpp
 *   @brief  
 *
 *    @date  07/10/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "SequenceTool.h"
#include "AccessEvent.h"
#include "AccessInfo.h"
#include "AcquireEvent.h"
#include "AcquireInfo.h"
#include "CallEvent.h"
#include "CallInfo.h"
#include "JoinEvent.h"
#include "JoinInfo.h"
#include "NewThreadEvent.h"
#include "NewThreadInfo.h"
#include "ReleaseEvent.h"
#include "ReleaseInfo.h"
#include "ReturnEvent.h"
#include "ReturnInfo.h"
#include "ThreadEndEvent.h"
#include "ThreadEndInfo.h"

#include "ShadowThread.h"

#include <utility>
#include <iostream>
#include <string>
#include <assert.h>

void SequenceTool::check_event_time(TIME new_event_time, 
	  						              std::string event_label) {

	std::cout << event_label << " EVENT" << std::endl;
	std::cout << "new_event_time" << new_event_time << std::endl;
	std::cout << "last_event_time" << last_event_time << std::endl;
	std::cout << "difference is " << (long long int) new_event_time  - (long long int) last_event_time << std::endl;
	assert(new_event_time >= last_event_time);;
} 

void SequenceTool::Access(const AccessEvent* event) {}

void SequenceTool::Acquire(const AcquireEvent* event) {}

void SequenceTool::Call(const CallEvent* event) {
	const CallInfo* _info(event->getInfo());
	std::string call_label = "CALL_" + _info->fnSignature;
	check_event_time(_info->callTime, call_label);
}

void SequenceTool::Join(const JoinEvent* event) {

	const JoinInfo* const _info = event->getInfo();
	std::string join_label = "JOIN_" + std::to_string(static_cast<unsigned>(_info->childThread->threadId)) + "_" + 
								       std::to_string(static_cast<unsigned>(_info->parentThread->threadId));
	check_event_time(_info->joinTime, join_label);	
}

void SequenceTool::NewThread(const NewThreadEvent* event) {
	const NewThreadInfo* _info(event->getInfo());
	std::string new_thread_label = "TS_" + std::to_string(static_cast<unsigned>(_info->childThread->threadId));
	check_event_time(_info->startTime, new_thread_label);
}

void SequenceTool::Release(const ReleaseEvent* event) {}

void SequenceTool::Return(const ReturnEvent* event) {
	const ReturnInfo* _info(event->getInfo());
	std::string return_label = "R_" + std::to_string(static_cast<unsigned>(_info->call));
	check_event_time(_info->endTime, return_label);
}

void SequenceTool::ThreadEnd(const ThreadEndEvent* event) {
	const ThreadEndInfo* _info(event->getInfo());
	std::string thread_end_label = "TE_" + std::to_string(static_cast<unsigned>(_info->id));
	check_event_time(_info->endTime, thread_end_label);
}
