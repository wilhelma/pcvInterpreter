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

/// @struct AcquireInfo
struct AcquireInfo {
  ShadowLock *lock;
  TIME acquireTime;

  explicit AcquireInfo(ShadowLock *lock, TIME acquireTime) noexcept
    : lock(lock), acquireTime(acquireTime) {}
};

#endif
