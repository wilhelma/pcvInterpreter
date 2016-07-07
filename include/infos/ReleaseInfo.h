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

struct ReleaseInfo {
	ShadowLock *lock;
	explicit ReleaseInfo(ShadowLock* lock) noexcept
		: lock(lock) {}
};

#endif
