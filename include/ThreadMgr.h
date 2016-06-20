/*
 * ThreadMgr.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef THREADMGR_H_
#define THREADMGR_H_

#include <map>
#include "ShadowThread.h"
#include "Types.h"

/******************************************************************************
 * ThreadMgr
 *****************************************************************************/
class ThreadMgr {
public:
	ThreadMgr() {}
	~ThreadMgr() {}

	ShadowThread* getThread(TRD_ID threadId);
	void threadJoined(TRD_ID threadId);

private:
	static ShadowThread::ThreadId currentThreadId_;
	typedef std::map<TRD_ID, ShadowThread*> TIdThreadMap_;
	
	TIdThreadMap_ tIdThreadMap_;

	// prevent generated functions
	ThreadMgr(const ThreadMgr&);
	ThreadMgr& operator=(const ThreadMgr&);
};


#endif /* THREADMGR_H_ */
