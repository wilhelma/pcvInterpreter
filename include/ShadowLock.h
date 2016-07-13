/*
 * ShadowLock.h
 *
 *  Created on: Sep 1, 2014
 *      Author: wilhelma
 */

#ifndef SHADOWLOCK_H_
#define SHADOWLOCK_H_

#include "Types.h"

/******************************************************************************
 * ShadowLock
 *****************************************************************************/
class ShadowLock {
public:

	typedef unsigned int LockId;
	ShadowLock(LockId lockId)
		: lockId(lockId)
	{};
	
	const LockId lockId;
//	TIME last_acquire_time;	
	
	bool operator < (const ShadowLock& other) const { return lockId < other.lockId; };

private:
	
	// prevent generated functions
	ShadowLock(const ShadowLock&);
	ShadowLock& operator=(const ShadowLock&);
};

#endif /* SHADOWLOCK_H_ */
