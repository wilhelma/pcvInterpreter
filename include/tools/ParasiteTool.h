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

	void Access(const AccessEvent* e) override;
	void Acquire(const AcquireEvent* e) override;
    void Call(const CallEvent* e) override;
	void Join(const JoinEvent* e) override;
	void NewThread(const NewThreadEvent* e) override;
	void Release(const ReleaseEvent* e) override;
	void Return(const ReturnEvent* e) override;
	void ThreadEnd(const ThreadEndEvent* e) override;

	void add_down_stack(TIME local_work, TIME strand_end_time);
	
	vertex_descr_type add_local_work(TIME strand_end_time, 
	  				   				 std::string end_vertex_label);

	void endProfileCalculations();

	/**
	*    @fn printOverallProfile()
	*    @brief Prints overall profile in a format to be decided. 
		 @todo Decide on format to print out profile information.
	*/
	void printOverallProfile();


	vertex_descr_type add_edge(TIME length, std::string end_vertex_label);

	void add_join_edges(vertex_descr_type start);

	TIME concur(TIME serial_time);

	void outputCallSites();

	void outputOverallProfile();
	
	/**
	*    @var main_stack
	*    @brief Contains a thread stack and a function stack to track events. 
	*/
	ParasiteTracker stacks;

	/**
	*    @var parasite_profile
	*    @brief Contains parallelism, work, and span after simulator finishes.
	*/
	parasite_profile_t parasite_profile;

	Work work;

	DAG thread_graph;

	std::string name;

	ParasiteJsonWriter jsonWriter;

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
