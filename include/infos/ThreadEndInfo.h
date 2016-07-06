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

#include "Types.h"

struct ThreadEndInfo {
	TIME endTime;
	TRD_ID id;

	explicit
	ThreadEndInfo(TIME endTime, TRD_ID id) noexcept
		: endTime(endTime), id(id) {}
};

#endif
