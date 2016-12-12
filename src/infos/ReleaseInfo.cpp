/**
 *
 *    @file  ReleaseInfo.cpp
 *   @brief  
 *
 *    @date  11/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ReleaseInfo.h"

#include "ShadowLock.h"
#include "ShadowLockMap.h"
#include "Types.h"

#include <iterator>
#include <memory>

ReleaseInfo::ReleaseInfo(ShadowLockMap::const_iterator lock_info, const TIME& release_time) :
    LockInfo(lock_info),
    ReleaseTime_(release_time)
{}

const TIME& ReleaseInfo::releaseTime() const noexcept {
    return ReleaseTime_;
}
