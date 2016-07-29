/**
 *
 *    @file  ParasiteTool.h
 *   @brief  Declarations for the class `ParasiteTool`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef PARASITE_TOOL_H_
#define PARASITE_TOOL_H_

#include <array>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "AccessEvent.h"
#include "AcquireEvent.h"
#include "CallEvent.h"
#include "CallSiteProfile.h"
#include "CallSiteSpanProfile.h"
#include "CallSiteWorkHashtable.h"
#include "CallSiteWorkProfile.h"
#include "Event.h"
#include "JoinEvent.h"
#include "NewThreadEvent.h"
#include "ReturnEvent.h"
#include "ReleaseEvent.h"
#include "ThreadEndEvent.h"

#include "Interpreter.h"
#include "ParasiteTracker.h"
#include "ShadowLock.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "Tool.h"

/**
*    @struct parasite_profile_t
*    @brief Work and span information describing entire program being profiled. 

*/
struct parasite_profile_t {

	/**
	*    @var work
	*    @brief The work (the total runtime, scaled) of all instructions
				executed in the program.
	*/
	TIME work;

	/**
	*    @var span
	*    @brief The span (the total runtime, scaled, on the criical path) 
				of all instructions executed in the program.
	*/
	TIME span;

	/**
	*    @var lock_span
	*    @brief The total lock span (the total runtime, scaled, on the criical
				path, spent while at least one lock is engaged) of all 
				instructions executed in the program.
	*/
	TIME lock_span;

	/**
	*    @var parallelism
	*    @brief The parallelism of the program. This is the work divided by span,
				which is an upper bound of the program's speedup on any number
				of processors.
	*/
	double parallelism;

	parasite_profile_t(): work(0), span(0), lock_span(0), parallelism(0) {}
};


/**
*
*    @class  ParasiteTool.h
*    @brief  Updates work and span variables when Events occur, to produce
			  a scalability profile for the entire program as well as 
			  each call site. 
*/

class ParasiteTool : public Tool {
 public:
	ParasiteTool();
	~ParasiteTool();

	void Access(const AccessEvent* e) override;
	void Acquire(const AcquireEvent* e) override;
    void Call(const CallEvent* e) override;
	void Join(const JoinEvent* e) override;
	void NewThread(const NewThreadEvent* e) override;
	void Release(const ReleaseEvent* e) override;
	void Return(const ReturnEvent* e) override;
	void ThreadEnd(const ThreadEndEvent* e) override;

	/**
	*    @fn printProfile()
	*    @brief Prints overall profile, and function profile information, in a format to be decided. 
		 @todo Decide on format to print out profile information.
	*/
	void printProfile();

	/**
	*    @fn printOverallProfile()
	*    @brief Prints overall profile in a format to be decided. 
		 @todo Decide on format to print out profile information.
	*/
	void printOverallProfile();


	/**
	*    @fn printCallSiteProfiles()
	*    @brief Prints call ite profile information, in a format to be decided. 
		 @todo Decide on format to print out profile information.
	*/
	void printCallSiteProfiles();
	
	/**
	*    @var main_stack
	*    @brief Contains a thread stack and a function stack to track events. 
	*/
	std::unique_ptr<ParasiteTracker> stacks;

	/**
	*    @var parasite_profile
	*    @brief Contains parallelism, work, and span after simulator finishes.
	*/
	std::unique_ptr<parasite_profile_t> parasite_profile;

	std::shared_ptr<CallSiteWorkHashtable> work_table;

	/**
	*    @var lock_hashtable
	*    @brief Maps lock IDs to the index of their respective functions in the 
				function stack. 
	*/
	std::unordered_map<unsigned int, int> lock_hashtable;


	/**
	*    @var last_function_call_time
	*    @brief Time stamp for the most recent function call in the simulator. 
	*/
	TIME last_function_call_time;


	/**
	*    @var last_function_return_time
	*    @brief Time stamp for the most recent function return in the simulator.
	*/
	TIME last_function_return_time;

	/**
	*    @var last_function_runtime
	*    @brief Duration of the last function that was called. 
	*/
	TIME last_function_runtime;

	/**
	*    @var last_thread_end_time
	*    @brief Time stamp for the last thread that has ended in the simulator.
	*/
	TIME last_thread_end_time;


	/**
	*    @var last_thread_runtime
	*    @brief Duration of the last function that was called. 
	*/
	TIME last_thread_runtime;

	/**
	*    @var last_thread_start_time
	*    @brief Time stamp for the start of the most recent thread in the simulator.
	*/
	TIME last_thread_start_time;


	/**
	*    @var last_lock_end_time
	*    @brief Tracks the last lock end time, for calculation of 
				lock span. 
	*/
	TIME lock_span_end_time;

	/**
	*    @var last_lock_start_time
	*    @brief Tracks the last lock start time, for calculation of 
				lock span. 
	*/
	TIME lock_span_start_time;

	/**
	*    @var last_lock_start_time
	*    @brief Tracks the time of the last event
	*/
	TIME last_event_time;


 private:
	// prevent generated functions --------------------------------------------
	ParasiteTool(const ParasiteTool&);
	ParasiteTool& operator=(const ParasiteTool&);
};

#endif  // PARASITE_TOOL_H_ 
