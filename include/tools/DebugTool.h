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

#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/CallEvent.h"
#include "fwd/JoinEvent.h"
#include "fwd/NewThreadEvent.h"
#include "fwd/ReleaseEvent.h"
#include "fwd/ReturnEvent.h"
#include "fwd/ThreadEndEvent.h"

#include "Tool.h"

/// @ingroup observers
/// @brief Simple debug tool that logs all the received events.
class DebugTool final : public Tool {
	public:
        /// Process AccessEvent.
		virtual void Access(const AccessEvent* e);
        /// Process AcquireEvent.
		virtual void Acquire(const AcquireEvent* e);
        /// Process CallEvent.
		virtual void Call(const CallEvent* e);
        /// Process JoinEvent.
		virtual void Join(const JoinEvent* e);
        /// Process NewThreadEvent.
		virtual void NewThread(const NewThreadEvent* e);
        /// Process ReleaseEvent.
		virtual void Release(const ReleaseEvent* e);
        /// Process ReturnEvent.
		virtual void Return(const ReturnEvent* e);
        /// Process ThreadEndEvent.
		virtual void ThreadEnd(const ThreadEndEvent* e);

        /// _Default_ constructor.
        explicit DebugTool() = default;
        /// _Default_ destructor.
		virtual ~DebugTool() = default;
};

#endif
