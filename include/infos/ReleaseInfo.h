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

#include "fwd/ShadowLock.h"

#include "Types.h"

struct ReleaseInfo {
    /// @brief The lock that is released.
    std::shared_ptr<const ShadowLock> lock;
    /// When was the lock released.
    TIME releaseTime;

    explicit ReleaseInfo(std::shared_ptr<const ShadowLock> lock,
                         TIME releaseTime) noexcept
        : lock(lock),
          releaseTime(releaseTime)
        {}
};

#endif
