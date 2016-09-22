/**
 *
 *    @file  SequenceTool.h
 *   @brief  
 *
 *    @date  09/22/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef  SEQUENCE_TOOL_H_
#define  SEQUENCE_TOOL_H_

#include "Tool.h"
#include "Types.h"
#include <string>

class SequenceTool : public Tool {
	public:
		virtual void Access(const AccessEvent* e);
		virtual void Acquire(const AcquireEvent* e);
		virtual void Call(const CallEvent* e);
		virtual void Join(const JoinEvent* e);
		virtual void NewThread(const NewThreadEvent* e);
		virtual void Release(const ReleaseEvent* e);
		virtual void Return(const ReturnEvent* e);
		virtual void ThreadEnd(const ThreadEndEvent* e);

		virtual ~SequenceTool() {};

		void check_event_time(TIME new_event_time, 
	  						  std::string event_label);

		TIME last_event_time;
};

#endif
