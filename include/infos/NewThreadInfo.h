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

#include <memory>

/// Information about a thread creation.
struct NewThreadInfo {
    /// The created thread.
	std::shared_ptr<const ShadowThread> childThread;
    /// The parent thread.
	std::shared_ptr<const ShadowThread> parentThread;
	TIME startTime;
	TIME runTime;

    /// @brief Constructor.
    /// @param child_trd
    /// @param parent_trd
    /// @param start_time
    /// @param run_time
	explicit NewThreadInfo(std::shared_ptr<const ShadowThread> child_thread,
                           std::shared_ptr<const ShadowThread> parent_thread,
			               const TIME& start_time,
			               const TIME& run_time) noexcept
		: childThread(child_thread),
          parentThread(parent_thread),
		  startTime(start_time),
          runTime(run_time)
        {}
};

#endif
