#include "ThreadMgr.h"
													   
TRD_ID ThreadMgr::currentThreadId_(0);

ShadowThread* ThreadMgr::getThread(TRD_ID threadId) {
	
	ShadowThread* thread = nullptr;
	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		thread = search->second;
	else {
		thread = new ShadowThread( (TRD_ID) ThreadMgr::currentThreadId_);
        ThreadMgr::currentThreadId_++;
		tIdThreadMap_.insert(std::make_pair(threadId, thread));
	}
	return thread;
}

void ThreadMgr::threadJoined(TRD_ID threadId) {

	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		delete search->second;
	tIdThreadMap_.erase(threadId);
}
