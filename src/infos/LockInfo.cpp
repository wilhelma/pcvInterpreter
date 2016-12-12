/**
 *
 *    @file  LockInfo.cpp
 *   @brief  
 *
 *    @date  11/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "LockInfo.h"

#include "ShadowLock.h"
#include "ShadowLockMap.h"
#include "Types.h"

const TIME& LockInfo::acquireTime() const noexcept {
    return Lock_->second->acquire_time;
}
