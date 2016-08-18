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
#include "Work.h"
#include "CallSiteWorkProfile.h"
#include "DAG.h"
#include "Event.h"
#include "Interpreter.h"
#include "JoinEvent.h"
#include "LockIntervals.h"
#include "NewThreadEvent.h"
#include "ParasiteJsonWriter.h"
#include "ParasiteProfile.h"
#include "ParasiteTracker.h"
#include "ReturnEvent.h"
#include "ReleaseEvent.h"
#include "ShadowLock.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ThreadEndEvent.h"
#include "Tool.h"
#include "Utility.h"

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

	void Acquire(const AcquireEvent* e) override;
    void Call(const CallEvent* e) override;
	void Join(const JoinEvent* e) override;
	void NewThread(const NewThreadEvent* e) override;
	void Release(const ReleaseEvent* e) override;
	void Return(const ReturnEvent* e) override;
	void ThreadEnd(const ThreadEndEvent* e) override;
	
	vertex_descr_type add_local_work(TIME strand_end_time, 
	  				   				 std::string end_vertex_label);

	void endProfileCalculations();

	void print_event_end(std::string event_name);
	void print_event_start(std::string event_name);

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

	void writeJson();

	vertex_descr_type add_edge(TIME length, std::string end_vertex_label);

	void add_join_edges(vertex_descr_type start);
	
	/**
	*    @var main_stack
	*    @brief Contains a thread stack and a function stack to track events. 
	*/
	std::unique_ptr<ParasiteTracker> stacks;

	/**
	*    @var parasite_profile
	*    @brief Contains parallelism, work, and span after simulator finishes.
	*/
	std::shared_ptr<parasite_profile_t> parasite_profile;

	std::shared_ptr<Work> work;

	DAG thread_graph;

	/**
	*    @var last_thread_start_time
	*    @brief Time stamp for the start of the most recent thread in the simulator.
	*/
	TIME last_thread_start_time;

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
