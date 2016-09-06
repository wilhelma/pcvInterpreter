/*
 * LockMgr.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef LOCKMGR_H_
#define LOCKMGR_H_

#include "Types.h"
#include "ShadowLock.h"

#include <map>

/// Lock manager.
/// @todo Properly document and implement this!
class LockMgr {
public:
	/// _Default_ constructor.
	explicit LockMgr() noexcept = default;
	/// _Default_ destructor.
	~LockMgr()                  = default;
	
	/// _Deleted_ copy constructor.
	LockMgr(const LockMgr&)            = delete; 
	/// _Deleted_ move constructor.
	LockMgr(LockMgr&&)                 = delete; 
	/// _Deleted_ copy assignment operator.
	LockMgr& operator=(const LockMgr&) = delete;
	/// _Deleted_ move assignment operator.
	LockMgr& operator=(LockMgr&&)      = delete;

	/// @todo Document this!
	ShadowLock* getLock(REF_ID refNo);
	/// @todo Document this!
	void lockDestroyed(REF_ID refNo);

private:
	/// @todo Document this!
	static ShadowLock::LockId currentLockId_;

	using MemLockMap_ = std::map<REF_ID, ShadowLock*>;
	/// @todo Document this!
	MemLockMap_ memLockMap_;

};


#endif /* LOCKMGR_H_ */
