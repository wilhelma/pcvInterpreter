#include "LockMgr.h"

ShadowLock::LockId LockMgr::currentLockId_ = 0;

ShadowLock* LockMgr::getLock(REF_ID refNo) {
	
	ShadowLock* lock = nullptr;
	/*auto search = memLockMap_.find(address);
	if (search != memLockMap_.end()) {
		lock = search->second;
	} else {
		lock = new ShadowLock(LockMgr::currentLockId_++);
	}
	return lock;*/

	auto search = memLockMap_.find(refNo);
	if (search != memLockMap_.end()) {
		lock = search->second;
	} else {
		lock = new ShadowLock(LockMgr::currentLockId_++);
		memLockMap_.insert(std::make_pair(refNo, lock));
	}
	return lock;
}

void LockMgr::lockDestroyed(REF_ID refNo) {
	
	auto search = memLockMap_.find(refNo);
	if (search != memLockMap_.end())
		delete search->second;
	memLockMap_.erase(refNo);
}
