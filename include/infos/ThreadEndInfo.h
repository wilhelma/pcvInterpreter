/**
 *
 *    @file  ThreadEndInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  THREAD_END_INFO_H_
#define  THREAD_END_INFO_H_

#include "fwd/ShadowThreadMap.h"

#include "ChildThreadInfo.h"
#include "Types.h"

#include <cassert>

/// @ingroup info
/// @brief Information about when a thread is done with its job.
class ThreadEndInfo : public ChildThreadInfo {
public:
    /// Constructor.
    /// @param child_thread_it The information about the child thread.
    /// @param end_time        The time when the thread finished its work.
    explicit ThreadEndInfo(ShadowThreadMap::const_iterator child_thread_it,
                           const TIME& end_time) noexcept
        : ChildThreadInfo(child_thread_it),
          EndTime_(end_time)
        {}

    /// Return the time when the thread has finished its work.
    const TIME& endTime() const noexcept
    { return EndTime_; }

private:
    /// The time when the thread finished its work.
    TIME EndTime_;
};

#endif
