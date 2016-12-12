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
#include "fwd/ShadowThreadMap.h"

#include "ChildThreadInfo.h"
#include "Types.h"

#include <memory>

/// @ingroup info
/// @brief Information about a thread creation.
class NewThreadInfo : public ChildThreadInfo {
public:
    /// @brief Constructor.
    /// @param child_thread_id  The ID of the created thread.
    /// @param start_time       The time when the thread was created.
    /// @param run_time         The runtime of the thread.
	explicit NewThreadInfo(ShadowThreadMap::const_iterator child_thread_info,
			               const TIME& run_time) noexcept
		: ChildThreadInfo(child_thread_info),
          RunTime_(run_time)
        {}

    /// Return the thread runtime.
    const TIME& runTime() const noexcept
    { return RunTime_; }

private:
    /// The thread runtime.
    TIME RunTime_;
};

#endif
