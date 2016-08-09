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

	TIME last_acquire_time;
	typedef unsigned int LockId;
	ShadowLock(LockId lockId)
		: last_acquire_time(0), lockId(lockId) 
	{};
	
	const LockId lockId;
	bool operator < (const ShadowLock& other) const { return lockId < other.lockId; };

private:
	
	// prevent generated functions
	ShadowLock(const ShadowLock&);
	ShadowLock& operator=(const ShadowLock&);
};

#endif /* SHADOWLOCK_H_ */
