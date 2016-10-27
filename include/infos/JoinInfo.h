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

#include <memory>

/// Information about a thread join.
struct JoinInfo {
    std::shared_ptr<const ShadowThread> childThread;
    std::shared_ptr<const ShadowThread> parentThread;
    TIME joinTime;

    explicit JoinInfo(std::shared_ptr<const ShadowThread> child_thread,
                      std::shared_ptr<const ShadowThread> parent_thread,
                      const TIME& join_time) noexcept
        : childThread(child_thread),
          parentThread(parent_thread),
          joinTime(join_time)
        {}
};

#endif
