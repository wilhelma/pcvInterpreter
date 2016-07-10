/**
 *
 *    @file  DebugTool.h
 *   @brief  
 *
 *    @date  07/10/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  DEBUG_TOOL_H_
#define  DEBUG_TOOL_H_

#include "Tool.h"

class DebugTool : public Tool {
	public:
		virtual void Access(const AccessEvent* e);
		virtual void Acquire(const AcquireEvent* e);
		virtual void Call(const CallEvent* e);
		virtual void Join(const JoinEvent* e);
		virtual void NewThread(const NewThreadEvent* e);
		virtual void Release(const ReleaseEvent* e);
		virtual void Return(const ReturnEvent* e);
		virtual void ThreadEnd(const ThreadEndEvent* e);

		virtual ~DebugTool() {};
};

#endif
