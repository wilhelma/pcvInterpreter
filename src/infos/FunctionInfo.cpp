/**
 *
 *    @file  FunctionInfo.cpp
 *   @brief  
 *
 *    @date  12/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "FunctionInfo.h"

#include "ShadowCallMap.h"
#include "ShadowCall.h"
#include "Function.h"
#include "Types.h"

const CAL_ID& FunctionInfo::callId() const noexcept
{ return Call_->first; }

const TIME& FunctionInfo::callTime() const noexcept
{ return Call_->second->callTime(); }

const TIME& FunctionInfo::runtime() const noexcept
{ return Call_->second->runtime(); }

const FunctionType FunctionInfo::functionType() const noexcept
{ return Call_->second->functionType(); }

const FUN_ID& FunctionInfo::functionId() const noexcept
{ return Call_->second->functionId(); }

const SEG_ID& FunctionInfo::segmentId() const noexcept
{ return Call_->second->segmentId(); }
