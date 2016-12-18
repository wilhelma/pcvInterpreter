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
#include "ShadowLock.h"

// Loggin system
#include "easylogging++.h"
//
//#include <utility>
//#include <iostream>

void DebugTool::Access(const AccessEvent* event) {
<<<<<<< HEAD
//	LOG(DEBUG)
//		<< "AccessEvent:\n"
//		<< " > Access Type:  " << static_cast<unsigned>(event->info()->type) << "\n"
//		<< " > Instruct. id: " << event->info()->instructionID << std::endl;
}

void DebugTool::Acquire(const AcquireEvent* event) {
//	LOG(DEBUG) << "AcquireEvent:" << event->info()->acquireTime << std::endl;
=======
	LOG(DEBUG) 
		<< "AccessEvent:\n"
		<< " > Access Type:  " << static_cast<unsigned>(event->info()->accessType()) << "\n"
		<< " > Instruct. id: " << event->info()->instructionId() << std::endl;
}

void DebugTool::Acquire(const AcquireEvent* event) {
	LOG(DEBUG) << "AcquireEvent:" << event->info()->acquireTime() << std::endl;
>>>>>>> [EventGenerator] creates and publishes events
}

void DebugTool::Call(const CallEvent* event) {
<<<<<<< HEAD
//	LOG(DEBUG)
//		<< "CallEvent:\n"
//        << " > Call Site:  " << event->info()->siteId << "\n"
//        << " > Start Time  " << event->info()->callTime << "\n"
//        << " > Runtime:    " << event->info()->runtime << "\n"
//		<< " > Fun. sig.:  " << event->info()->fnSignature << "\n"
//		<< " > Fun. type:  " << static_cast<unsigned>(event->info()->fnType) << "\n"
//		<< " > File name:  " << event->info()->fileName << "\n"
//		<< " > File path:  " << event->info()->filePath << std::endl;
=======
	LOG(DEBUG) 
		<< "CallEvent:\n"
        << " > Call Site:  " << event->info()->siteId() << "\n"
        << " > Start Time  " << event->info()->callTime() << "\n"
        << " > Runtime:    " << event->info()->runtime() << "\n"
		<< " > Fun. sig.:  " << event->info()->functionSignature() << "\n"
		<< " > Fun. type:  " << static_cast<unsigned>(event->info()->functionType()) << std::endl;
>>>>>>> [ShadowCall] create ShadowCall{,Map}
}


void DebugTool::Join(const JoinEvent* event) {
//	const JoinInfo& ji = *(event->getInfo());
//	LOG(DEBUG) << "JoinEvent:" << std::endl;
}

void DebugTool::NewThread(const NewThreadEvent* event) {
<<<<<<< HEAD
//	LOG(DEBUG)
//		<< "NewThreadEvent:\n"
//        << " > Thread id:   " << event->info()->childThread->threadId << std::endl
//        << " > Start time:  " << event->info()->startTime << "\n"
//        << " > Runtime      " << event->info()->runTime << std::endl;
}

void DebugTool::Release(const ReleaseEvent* event) {
//	LOG(DEBUG) << "ReleaseEvent:" << event->info()->releaseTime << std::endl;
=======
	LOG(DEBUG) 
		<< "NewThreadEvent:\n"
        << " > Thread id:   " << event->info()->childThreadId() << std::endl
        << " > Start time:  " << event->info()->startTime() << "\n"
        << " > Runtime      " << event->info()->runTime() << std::endl;
}

void DebugTool::Release(const ReleaseEvent* event) {
	LOG(DEBUG) << "ReleaseEvent:" << event->info()->releaseTime() << std::endl;
>>>>>>> [EventGenerator] creates and publishes events
}

void DebugTool::Return(const ReturnEvent* event) {
<<<<<<< HEAD
//	LOG(DEBUG)
//        << "ReturnEvent:\n"
//        << " > Call ID:     " << event->info()->call << "\n"
//        << " > Function ID: " << event->info()->function << "\n"
//        << " > End time:    " << event->info()->endTime << std::endl;
=======
	LOG(DEBUG) 
        << "ReturnEvent:\n"
        << " > Call ID:     " << event->info()->callId() << "\n"
        << " > Function ID: " << event->info()->functionId() << "\n"
        << " > End time:    " << event->info()->endTime() << std::endl;
>>>>>>> [FunctionInfo] is now base for ReturnInfo and CallInfo
}

void DebugTool::ThreadEnd(const ThreadEndEvent* event) {
<<<<<<< HEAD
//	LOG(DEBUG)
//        << "ThreadEndEvent:\n"
//		<< " > Thread ID: " << event->info()->id << "\n"
//		<< " > End time:  " << event->info()->endTime << std::endl;
=======
	LOG(DEBUG) 
        << "ThreadEndEvent:\n"
		<< " > Thread ID: " << event->info()->childThreadId() << "\n"
		<< " > End time:  " << event->info()->endTime() << std::endl;
>>>>>>> [EventGenerator] creates and publishes events
}
