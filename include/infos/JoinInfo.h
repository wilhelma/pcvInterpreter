/**
 *
 *    @file  JoinInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  JOIN_INFO_H_
#define  JOIN_INFO_H_

#include "fwd/ShadowThread.h"
#include "Types.h"

struct JoinInfo {
	const ShadowThread* childThread;
	const ShadowThread* parentThread;
  TIME joinTime;

  explicit JoinInfo(const ShadowThread* childThread,
                    const ShadowThread* parentThread,
                    const TIME& joinTime) noexcept
    : childThread(childThread), parentThread(parentThread),
      joinTime(joinTime) {}
};

#endif
