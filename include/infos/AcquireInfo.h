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

struct AcquireInfo {
       ShadowLock *lock;
       AcquireInfo(ShadowLock *lock) : lock(lock) {}
};

#endif
