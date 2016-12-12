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
/// @todo Document this!
/// @todo Complete implementation: it doesn't make sense to
/// define comparison operators if LockId_ is publicly accessible!
struct ShadowLock {
    const REF_ID lock_id;
    const TIME   acquire_time;	

    /// @brief Constructor.
    /// @param lock_id      The ID of the reference to the lock.
    /// @param acquire_time Time when the lock was acquired.
    explicit ShadowLock(const REF_ID& lock_id,
                        const TIME& acquire_time) noexcept
        : lock_id(lock_id),
          acquire_time(acquire_time)
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


    /// Less-than operator.
    friend const bool operator<(const ShadowLock& lhs, const ShadowLock& rhs)
    { return lhs.lock_id < rhs.lock_id; }

    /// Equality operator.
    friend const bool operator==(const ShadowLock& lhs, const ShadowLock& rhs)
    { return lhs.lock_id == rhs.lock_id; }
};

/// Greater-than operator.
inline const bool operator>=(const ShadowLock& lhs, const ShadowLock& rhs)
{ return !(lhs < rhs); }

#endif
