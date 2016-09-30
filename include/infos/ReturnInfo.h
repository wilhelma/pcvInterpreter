/**
 *
 *    @file  ReturnInfo.h
 *   @brief 
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RETURN_INFO_INC
#define  RETURN_INFO_INC

#include "Types.h"

typedef struct ReturnInfo {
  CAL_ID call;
  FUN_ID function;
  TIME   endTime;

  explicit ReturnInfo(const CAL_ID& call,
                      const FUN_ID function,
                      const TIME& endTime) noexcept
    : call(call), function(function), endTime(endTime)
  {}
} ReturnInfo;

#endif
