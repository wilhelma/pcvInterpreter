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
	TIME   endTime;

	explicit
	ReturnInfo(CAL_ID call, TIME endTime) noexcept
		: call(call), endTime(endTime)
	{}
} ReturnInfo;

#endif
