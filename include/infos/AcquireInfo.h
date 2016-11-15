/**
 *
 *    @file  AcquireInfo.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACQUIRE_INFO_H_
#define  ACQUIRE_INFO_H_

#include "fwd/ShadowLock.h"

#include "Types.h"

#include <memory>

/// @struct AcquireInfo
struct AcquireInfo {
    /// The lock that is being held.
    std::shared_ptr<const ShadowLock> lock;
    /// When was the lock acquired.
    TIME acquireTime;

    explicit AcquireInfo(std::shared_ptr<const ShadowLock> lock,
                         TIME acquireTime) noexcept
        : lock(lock),
          acquireTime(acquireTime)
        {}
};

#endif
