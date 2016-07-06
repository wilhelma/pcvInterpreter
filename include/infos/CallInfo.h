/**
 *
 *    @file  CallInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_INFO_H_
#define  CALL_INFO_H_

#include "Function.h"
#include "Types.h"

struct CallInfo {
	CALLSITE siteId;
	TIME runtime;
	FUN_SG fnSignature;
	SEG_ID segment;
	FunctionType fnType;
	FIL_PT fileName;
	FIL_PT filePath;

	explicit
		CallInfo(CALLSITE SiteId,
				TIME Runtime,
				FUN_SG FnSignature,
				SEG_ID Segment,
				FunctionType FnType,
				FIL_PT FileName,
				FIL_PT FilePath) noexcept
		: siteId(SiteId), runtime(Runtime), fnSignature(FnSignature),
		segment(Segment), fnType(FnType), fileName(FileName), filePath(FilePath) {}
};

#endif
