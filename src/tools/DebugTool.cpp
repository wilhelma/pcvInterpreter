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

#include "ShadowThread.h"

#include <boost/log/trivial.hpp>
#include <utility>
#include <iostream>

void DebugTool::Access(const AccessEvent* event) {
	const AccessInfo& ai = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "AccessEvent:\n"
		<< " > Access Type:  " << static_cast<unsigned>(ai.type) << "\n"
		<< " > Instruct. id: " << ai.instructionID << "\n";
}

void DebugTool::Acquire(const AcquireEvent* event) {
  const AcquireInfo& ai = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
    << "AcquireEvent:" << ai.acquireTime << std::endl;
}

void DebugTool::Call(const CallEvent* event) {
	const CallInfo& ci = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "CallEvent:\n"
        << " > Call Site:  " << ci.siteId << "\n"
        << " > Start Time  " << ci.callTime << "\n"
        << " > Runtime:    " << ci.runtime << "\n"
		<< " > Fun. sig.:  " << ci.fnSignature << "\n"
		<< " > Fun. type:  " << static_cast<unsigned>(ci.fnType) << "\n"
		<< " > File name:  " << ci.fileName << "\n"
		<< " > File path:  " << ci.filePath << "\n";
}


void DebugTool::Join(const JoinEvent* event) {
//	const JoinInfo& ji = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "JoinEvent:";
}

void DebugTool::NewThread(const NewThreadEvent* event) {
	const NewThreadInfo& nti = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "NewThreadEvent:\n"
        << " > Thread id: " << nti.childThread->threadId << std::endl
        << " > Start time:  " << nti.startTime << "\n"
        << " > Runtime      " << nti.runTime << "\n";
}

void DebugTool::Release(const ReleaseEvent* event) {
  const ReleaseInfo& ri = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
    << "ReleaseEvent:" << ri.releaseTime << std::endl;
}

void DebugTool::Return(const ReturnEvent* event) {
	const ReturnInfo& ri = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
		<< "ReturnEvent:\n"
    << " > Call ID: " << ri.call << "\n"
    << " > Function ID: " << ri.function << "\n"
		<< " > End time: " << ri.endTime << "\n";
}

void DebugTool::ThreadEnd(const ThreadEndEvent* event) {
	const ThreadEndInfo& tei = *(event->getInfo());
	BOOST_LOG_TRIVIAL(debug) 
        << "ThreadEndEvent:\n"
		<< " > Thread ID: " << tei.id << "\n"
		<< " > End time:  " << tei.endTime << "\n";
}
