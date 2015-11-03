/*
 * ThreadMgr.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef THREADMGR_H_
#define THREADMGR_H_

#include "DataModel.h"

#include <map>
#include "ShadowThread.h"

/******************************************************************************
 * ThreadMgr
 *****************************************************************************/
class ThreadMgr {
public:
	ThreadMgr() {}
	~ThreadMgr() {}

	ShadowThread* getThread(ThreadId threadId);
	void threadJoined(ThreadId threadId);

private:
	static ShadowThread::ThreadId currentThreadId_;
	typedef std::map<ThreadId, ShadowThread*> TIdThreadMap_;
	
	TIdThreadMap_ tIdThreadMap_;

	// prevent generated functions
	ThreadMgr(const ThreadMgr&);
	ThreadMgr& operator=(const ThreadMgr&);
};


#endif /* THREADMGR_H_ */
