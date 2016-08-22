/**
 *
 *    @file  NewThreadInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  NEW_THREAD_INFO_H_
#define  NEW_THREAD_INFO_H_

#include "fwd/ShadowThread.h"

#include "Types.h"

struct NewThreadInfo {
	const ShadowThread* const childThread;
	const ShadowThread* const parentThread;
	TIME startTime;
	TIME runTime;

	explicit NewThreadInfo(const ShadowThread* const childThread,
			const ShadowThread* const parentThread,
			const TIME& startTime,
			const TIME& runTime) noexcept
		: childThread(childThread), parentThread(parentThread),
		 startTime(startTime), runTime(runTime) {}
};

#endif
