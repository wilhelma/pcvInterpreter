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

void DebugTool::Access(const AccessEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::Acquire(const AcquireEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::Call(const CallEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::Join(const JoinEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::NewThread(const NewThreadEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::Release(const ReleaseEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::Return(const ReturnEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}

void DebugTool::ThreadEnd(const ThreadEndEvent* event) {
	LOG(DEBUG) << *event << std::endl;
}
