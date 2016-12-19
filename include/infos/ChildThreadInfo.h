/**
 *
 *    @file  ChildThreadInfo.h
 *   @brief  
 *
 *    @date  11/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CHILD_THREAD_INFO_H_
#define  CHILD_THREAD_INFO_H_

#include "fwd/ShadowThread.h"

#include "ShadowThreadMap.h"
#include "Types.h"

#include <memory>

/// @ingroup info
/// @brief Information about the child thread on a thread join or creation event.
class ChildThreadInfo {
public:
    /// @brief Constructor.
    /// @param thread_info The information about the child thread.
    explicit ChildThreadInfo(ShadowThreadMap::const_iterator thread_info) noexcept
        : ChildThread_(thread_info)
        {}

    /// Return the ID of the child thread.
    const TRD_ID& childThreadId() const noexcept;

    /// Return the pointer to the child thread ShadowThread.
    const std::shared_ptr<const ShadowThread>& childThread() const noexcept;

    /// Return the thread creation time.
    const TIME& startTime() const noexcept;

private:
    /// The information about the forked thread.
    ShadowThreadMap::const_iterator ChildThread_;
};

#endif
