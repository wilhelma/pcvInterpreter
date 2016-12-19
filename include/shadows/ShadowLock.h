/*
 * ShadowLock.h
 *
 *  Created on: Sep 1, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_LOCK_H_
#define SHADOW_LOCK_H_

#include "Types.h"

/// @defgroup shadows
/// @brief 
/// @todo What are the shadows?

/// @ingroup shadows
/// @brief Shadow information for a memory lock.
class ShadowLock {
public:
    /// @brief Constructor.
    /// @param lock_id      The ID of the reference to the lock.
    /// @param acquire_time Time when the lock was acquired.
    constexpr explicit ShadowLock(const REF_ID& lock_id,
                                  const TIME& acquire_time) noexcept
        : LockId_(lock_id),
          AcquireTime_(acquire_time)
        {}

    /// _Default_ destructor.
    ~ShadowLock() = default;

    /// _Deleted_ copy constructor.
    ShadowLock(const ShadowLock&) = delete;
    /// _Deleted_ move constructor.
    ShadowLock(ShadowLock&&)      = delete;
    /// _Deleted_ copy assignment operator.
    ShadowLock& operator=(const ShadowLock&) = delete;
    /// _Deleted_ move assignment operator.
    ShadowLock& operator=(ShadowLock&&)      = delete;

    /// Returns the ID of the lock.
    constexpr const REF_ID& lockId() const noexcept
    { return LockId_; }

    /// Returns the time when the lock was acquired.
    constexpr const TIME& acquireTime() const noexcept
    { return AcquireTime_; }

private:
    /// The ID of the lock.
    const REF_ID LockId_;
    /// The time when the lock was acquired.
    const TIME   AcquireTime_;	
};

/// Less-than operator.
constexpr const bool operator<(const ShadowLock& lhs, const ShadowLock& rhs)
{ return lhs.lockId() < rhs.lockId(); }

#endif
