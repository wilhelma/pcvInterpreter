#include "ThreadMgr.h"
													   
ShadowThread::ThreadId ThreadMgr::currentThreadId_ = 0;

ShadowThread* ThreadMgr::getThread(ThreadId threadId) {
	
	ShadowThread* thread = nullptr;
	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		thread = search->second;
	else {
		thread = new ShadowThread(ThreadMgr::currentThreadId_++);
		tIdThreadMap_.insert(std::make_pair(threadId, thread));
	}
	return thread;
}

void ThreadMgr::threadJoined(ThreadId threadId) {

	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		delete search->second;
	tIdThreadMap_.erase(threadId);
}