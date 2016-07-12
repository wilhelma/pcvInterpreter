#include "ThreadMgr.h"
													   
TRD_ID ThreadMgr::currentThreadId_ = (TRD_ID) 1;

ShadowThread* ThreadMgr::getThread(TRD_ID threadId) {
	
	ShadowThread* thread = nullptr;
	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end()) {
		thread = search->second;
		printf("thread already exists \n");
	}
	else {
		thread = new ShadowThread( (TRD_ID) ThreadMgr::currentThreadId_);
        ThreadMgr::currentThreadId_++;
		tIdThreadMap_.insert(std::make_pair(threadId, thread));
		printf("thread is being created \n");
	}
	return thread;
}

void ThreadMgr::threadJoined(TRD_ID threadId) {

	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		delete search->second;
	tIdThreadMap_.erase(threadId);
}
