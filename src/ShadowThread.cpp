#include "ShadowThread.h"

ShadowThread::ShadowThread(ThreadId threadId) : threadId(threadId) {}
	
bool ShadowThread::operator < (const ShadowThread& other) const	{
	return (threadId< other.threadId);
}