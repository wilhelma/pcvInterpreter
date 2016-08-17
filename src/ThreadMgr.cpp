#include "ThreadMgr.h"

// Initialize the static data member.
TRD_ID ThreadMgr::currentThreadId_(0);

const ShadowThread* const ThreadMgr::getThread(const TRD_ID& threadId) {
	const ShadowThread* thread = nullptr;
	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		thread = search->second;
	else {
		thread = new ShadowThread(ThreadMgr::currentThreadId_);
        ThreadMgr::currentThreadId_++;
		tIdThreadMap_.insert(std::make_pair(threadId, thread));
	}
	return thread;
}

void ThreadMgr::threadJoined(const TRD_ID& threadId) {
	auto search = tIdThreadMap_.find(threadId);
	if (search != tIdThreadMap_.end())
		delete search->second;
	tIdThreadMap_.erase(threadId);
}
