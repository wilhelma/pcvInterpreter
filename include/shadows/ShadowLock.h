/*
 * ShadowLock.h
 *
 *  Created on: Sep 1, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_LOCK_H_
#define SHADOW_LOCK_H_

#include "Types.h"

/// @todo Document this!
/// @todo Complete implementation: it doesn't make sense to
/// define comparison operators if LockId_ is publicly accessible!
struct ShadowLock {
	/// @typedef LockId
	using LockId = unsigned int;
	/// @todo Document this!
	const LockId lockId;
	TIME last_acquire_time;	

	/// Constructor.
	explicit ShadowLock(LockId lockId) noexcept
		: lockId(lockId), last_acquire_time(0)
	{};
	/// _Default_ destructor.
	~ShadowLock() = default;

	/// _Deleted_ copy constructor.
	ShadowLock(const ShadowLock&)            = delete;
	/// _Deleted_ move constructor.
	ShadowLock(ShadowLock&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	ShadowLock& operator=(const ShadowLock&) = delete;
	/// _Deleted_ move assignment operator.
	ShadowLock& operator=(ShadowLock&&)      = delete;
	
	
	/// @brief Less-than operator.
	/// @details Compares the `lockId`s.
	friend const bool operator<(const ShadowLock& lhs, const ShadowLock& rhs)
	{ return lhs.lockId < rhs.lockId; };
	
	/// @brief Equality operator.
	/// @details Compares the `lockId`s.
	friend const bool operator==(const ShadowLock& lhs, const ShadowLock& rhs)
	{ return lhs.lockId == rhs.lockId; };
};

/// @brief Greater-than operator.
/// @return Negation of `Shadowlock::operator<()`.
inline const bool operator>=(const ShadowLock& lhs, const ShadowLock& rhs)
{ return !(lhs < rhs); };

#endif
