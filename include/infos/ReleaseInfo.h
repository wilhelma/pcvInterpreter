/**
 *
 *    @file  ReleaseInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RELEASE_INFO_H_
#define  RELEASE_INFO_H_

#include "fwd/ShadowLockMap.h"

#include "LockInfo.h"
#include "Types.h"

/// @ingroup info
/// @brief Information about the release of a memory lock.
/// @todo This should also remove the entry from the map when it's done.
class ReleaseInfo : public LockInfo {
public:
    /// @brief Constructor.
    /// @param lock_info Iterator to the ShadowLockMap entry where the lock information is stored.
    /// @param release_time The time when the lock was released.
    explicit ReleaseInfo(ShadowLockMap::const_iterator lock_info,
                         const TIME& release_time) noexcept
        : LockInfo(lock_info),
          ReleaseTime_(release_time)
        {}

    /// Return the time when the lock was released.
    const TIME& releaseTime() const noexcept
    { return ReleaseTime_; }

private:
    /// When the lock was released.
    TIME ReleaseTime_;
};

#endif
