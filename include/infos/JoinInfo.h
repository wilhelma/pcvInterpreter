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

#include "fwd/ShadowThreadMap.h"

#include "ChildThreadInfo.h"
#include "Types.h"

/// @ingroup info
/// @brief Information about a thread join.
/// @todo Remove child thread from the map on destruction.
class JoinInfo : public ChildThreadInfo {
public:
    /// @brief Constructor.
    /// @param child_thread_info The information about the joined thread.
    /// @param join_time         The time when the child thread was joined to the parent one.
    explicit JoinInfo(ShadowThreadMap::const_iterator child_thread_info,
                      const TIME& join_time) noexcept
        : ChildThreadInfo(child_thread_info),
          JoinTime_(join_time)
        {}

    /// Return the time when the thread was joined.
    const TIME& joinTime() const noexcept
    { return JoinTime_; }

private:
    /// Time when the child thread was joined to the parent one.
    TIME JoinTime_;
};

#endif
