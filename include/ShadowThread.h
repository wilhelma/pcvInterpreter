/*
 * ShadowThread.h
 *
 *  Created on: Sep 1, 2014
 *      Author: wilhelma
 */

#ifndef SHADOWTHREAD_H_
#define SHADOWTHREAD_H_

#include "Event.h"
#include "Types.h"

/******************************************************************************
 * ShadowThread
 *****************************************************************************/
class ShadowThread {
public:

	ShadowThread(TRD_ID threadId)
		: threadId(threadId)
	{};

	TRD_ID threadId;
	CALLSITE currentCallSiteID;
	FUN_SG currentFunctionSignature; 

	bool operator < (const ShadowThread& other) const { return threadId < other.threadId; };

private:

	// prevent generated functions
	ShadowThread(const ShadowThread&);
	ShadowThread& operator=(const ShadowThread&);
};

#endif /* SHADOWTHREAD_H_ */
