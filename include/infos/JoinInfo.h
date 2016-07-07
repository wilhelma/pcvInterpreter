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

struct JoinInfo {
	const ShadowThread* childThread;
	const ShadowThread* parentThread;

	explicit JoinInfo(const ShadowThread* childThread, const ShadowThread* parentThread) noexcept
		: childThread(childThread), parentThread(parentThread) {}
};

#endif
