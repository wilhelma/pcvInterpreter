/*
 * LockMgr.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef LOCKMGR_H_
#define LOCKMGR_H_

#include "DataModel.h"
#include "ShadowLock.h"

#include <map>

/******************************************************************************
 * LockMgr
 *****************************************************************************/
class LockMgr {
public:
	LockMgr() {}
	~LockMgr() {}

	ShadowLock* getLock(RefNo refNo);
	void lockDestroyed(RefNo refNo);

private:
	static ShadowLock::LockId currentLockId_;
	typedef std::map<RefNo, ShadowLock*> MemLockMap_;
	
	MemLockMap_ memLockMap_;

	// prevent generated functions
	LockMgr(const LockMgr&);
	LockMgr& operator=(const LockMgr&);
};


#endif /* LOCKMGR_H_ */
