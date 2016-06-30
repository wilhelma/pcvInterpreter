/**
 *
 *    @file  CallEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_EVENT_H_
#define  CALL_EVENT_H_

#include "Function.h"
#include "Types.h"
#include "ShadowThread.h"

typedef struct CallInfo {
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
} CallInfo;

class CallEvent : public Event {
	public:
		CallEvent(const ShadowThread *thread,
				  const CallInfo *info) :
			Event(thread), _info(info) {};

		virtual Events getEventType() const override final { return Events::CALL; };
		const CallInfo* const getCallInfo() const { return _info; };

	private:
		const CallInfo* const _info;

		// prevent generated functions
		CallEvent(const CallEvent&);
		CallEvent& operator=(const CallEvent&);
};

#endif
