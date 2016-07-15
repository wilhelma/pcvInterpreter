/**
 *
 *    @file  FunctionTrackerTool.h
 *   @brief  Header file for the class `FunctionTrackerTool`
 *
 *    @date  04/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */


#ifndef  FUNCTIONTRACKERTOOL_H_
#define  FUNCTIONTRACKERTOOL_H_

#include "Tool.h"

#include "Event.h"
#include "CallEvent.h"
#include "NewThreadEvent.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class FunctionTrackerTool: public Tool {
	public:
		FunctionTrackerTool (std::string outFileName = "functionGraph.dot", std::string graphName = "graph");
		~FunctionTrackerTool();

		virtual void Call(const CallEvent* e) final override;
		virtual void NewThread(const NewThreadEvent* e) final override;

		// get the number of recorded calls
		unsigned int getCalls() const { return calls; };

	private:
		std::string   _outFileName; ///< @brief Output file name.
		std::ofstream _outFile;     ///< @brief Output file.

		std::string   _graphName;   ///< @brief Graph name.

		int _lastSegmentNumber = -1; ///< @brief Tracks last segment number

		unsigned int calls = 0;      ///< @brief Number of recorded calls

		// no longer accessible from outside
		virtual void Access(const AccessEvent* e) final override {};
		virtual void Acquire(const AcquireEvent* e) final override {};
		virtual void Join(const JoinEvent* e) final override {};
		virtual void Release(const ReleaseEvent* e) final override {};
		virtual void Return(const ReturnEvent* e) final override {};
		virtual void ThreadEnd(const ThreadEndEvent* e) final override {};
};

#endif /* FUNCTIONTRACKERTOOL_H_ */
