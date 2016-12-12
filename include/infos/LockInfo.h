/**
 *
 *    @file  LockInfo.h
 *   @brief  
 *
 *    @date  11/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOCK_INFO_H_
#define  LOCK_INFO_H_

#include "ShadowLockMap.h"
#include "Types.h"

/// @ingroup info
/// @brief Information about a lock being held or released.
class LockInfo {
public:

    /// @brief Constructor.
    /// @param shadow_lock_map_iterator Iterator to the ShadowLockMap entry.
    explicit LockInfo(ShadowLockMap::const_iterator shadow_lock_map_iterator) noexcept
        : Lock_(shadow_lock_map_iterator)
    {}

    /// Return the lock ID.
    const REF_ID& lockId() const noexcept
    { return Lock_->first; }

    /// Return the ShadowLock.
    const std::shared_ptr<const ShadowLock>& lock() const noexcept
    { return Lock_->second; }

    /// Return the time when the lock was acquired.
    const TIME& acquireTime() const noexcept;

protected:
    /// The lock being held or released.
    ShadowLockMap::const_iterator Lock_;
};

#endif
