/**
 *
 *    @file  ChildThreadInfo.cpp
 *   @brief  
 *
 *    @date  11/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ChildThreadInfo.h"

#include "ShadowThread.h"
#include "ShadowThreadMap.h"
#include "Types.h"

const TRD_ID& ChildThreadInfo::childThreadId() const noexcept
{ return ChildThread_->first; }

const std::shared_ptr<const ShadowThread>& ChildThreadInfo::childThread() const noexcept
{ return ChildThread_->second; }

const TIME& ChildThreadInfo::startTime() const noexcept
{ return childThread()->StartTime_; }
