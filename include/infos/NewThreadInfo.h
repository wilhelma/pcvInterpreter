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
	ShadowThread* childThread;
	ShadowThread* parentThread;
	NUM_CYCLES numCycles;
	TIME startTime;
	TIME endTime;

	explicit NewThreadInfo(ShadowThread* childThread,
			ShadowThread* parentThread, NUM_CYCLES numCycles,
			const TIME_STRING& startTimeString,
			const TIME_STRING& endTimeString) noexcept
		: childThread(childThread), parentThread(parentThread),
		numCycles(numCycles),
		startTime(timeStringToTime(startTimeString)),
		endTime(timeStringToTime(endTimeString)) {}
};

#endif
