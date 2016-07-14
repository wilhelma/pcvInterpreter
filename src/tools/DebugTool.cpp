/**
 *
 *    @file  DebugTool.cpp
 *   @brief  
 *
 *    @date  07/10/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "DebugTool.h"
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

#include <boost/log/trivial.hpp>
#include <utility>
#include <iostream>

void DebugTool::Access(const AccessEvent* event) {
	const AccessInfo& ai = *(event->getAccessInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "AccessEvent:\n"
		<< " > Access Type:  " << static_cast<unsigned>(ai.type) << "\n"
		<< " > Instruct. id: " << ai.instructionID << "\n";
}

void DebugTool::Acquire(const AcquireEvent* event) {
//	const AcquireInfo& ai = *(event->getAcquireInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "AcquireEvent:";
}

void DebugTool::Call(const CallEvent* event) {
	const CallInfo& ci = *(event->getCallInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "CallEvent:\n"
		<< " > Call Site:  " << ci.siteId << "\n"
		<< " > Time:       " << ci.runtime << "\n"
		<< " > Fun. sig.:  " << ci.fnSignature << "\n"
		<< " > Fun. type:  " << static_cast<unsigned>(ci.fnType) << "\n"
		<< " > File name:  " << ci.fileName << "\n"
		<< " > File path:  " << ci.filePath << "\n";
}


void DebugTool::Join(const JoinEvent* event) {
//	const JoinInfo& ji = *(event->getJoinInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "JoinEvent:";
}

void DebugTool::NewThread(const NewThreadEvent* event) {
	const NewThreadInfo& nti = *(event->getNewThreadInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "NewThreadEvent:\n"
		// << " > Num. cycles: " << nti.runtime << "\n"
		<< " > Start time:  " << nti.startTime << "\n";
}

void DebugTool::Release(const ReleaseEvent* event) {
//	const ReleaseInfo& ri = *(event->getReleaseInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "ReleaseEvent:";
}

void DebugTool::Return(const ReturnEvent* event) {
	const ReturnInfo& ri = *(event->getReturnInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "ReturnEvent:\n"
		<< " > Call ID: " << ri.call << "\n"
		<< " > End time: " << ri.endTime << "\n";
}

void DebugTool::ThreadEnd(const ThreadEndEvent* event) {
	const ThreadEndInfo& tei = *(event->getThreadEndInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "ThreadEvent:\n"
		<< " > Thread ID: " << tei.id << "\n"
		<< " > End time:  " << tei.endTime << "\n";
}
