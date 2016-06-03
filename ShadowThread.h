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
	typedef TRD_ID ThreadId;

	ShadowThread(ThreadId threadId);
	const ThreadId threadId;

	CALLSITE currentCallSiteID;
	FUN_SG currentFunctionSignature; 

	bool operator < (const ShadowThread& other) const;

	//template<typename Key, typename Value> static Decoration<Key, Value> makeDec(const Value& init) {
	//	return Decoration<Key, Value>(init);	
	// }

private:

	// prevent generated functions
	ShadowThread(const ShadowThread&);
	ShadowThread& operator=(const ShadowThread&);
};

#endif /* SHADOWTHREAD_H_ */
