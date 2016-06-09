#include "ShadowLock.h"

ShadowLock::ShadowLock(LockId lockId) : lockId(lockId) {}

ShadowLock::~ShadowLock() {}
							
bool ShadowLock::operator < (const ShadowLock& other) const	{
	return (lockId< other.lockId);
}