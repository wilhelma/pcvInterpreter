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
#include "CallEvent.h"
#include "CallSiteProfile.h"
#include "EndProfiles.h"
#include "Event.h"
#include "JoinEvent.h"
#include "NewThreadEvent.h"
#include "ReturnEvent.h"
#include "ThreadEndEvent.h"

#include "Interpreter.h"
#include "ParasiteTracker.h"
#include "ShadowLock.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "Tool.h"


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

	void create(const Event* e);
	void join(const Event* e);
	void acquire(const Event* e);
	void release(const Event* e);
	void access(const Event* e);
	void call(const Event* e);
	void returnOfCalled(const Event* e);
	void threadEnd(const Event* e);

	/**
	*    @fn returnOperations()
	*    @brief Performs Parasite algorithm operations needed for function returns, 
				in both threadEnd and returnOfCalled events. 
	*/
	void returnOperations(double local_work);

	/**
	*    @fn printProfile()
	*    @brief Prints overall profile, and function profile information, in a format to be decided. 
		 @todo Decide on format to print out profile information.
	*/
	void printProfile();

	/**
	*    @fn getEndProfile()
	*    @brief Converts the information contained in main_stack
				to the end profiles for each call site in (end_call_site_profile_hashtable)
				and the overall program (parasite_profile).
	*/
	void getEndProfile();
	
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

	/**
	*    @var end_call_site_profile_hashtable 
	*    @brief Contains a detailed profile of work and span measurements
				for each call site after simulator finishes.
	*/
	std::unique_ptr<call_site_end_hashtable_t> end_call_site_profile_hashtable;


	/**
	*    @var lock_hashtable
	*    @brief Maps lock IDs to the index of their respective functions in the 
				function stack. 
	*/
	std::unordered_map<unsigned int, int> lock_hashtable;


	/**
	*    @var last_strand_start_time
	*    @brief Time stamp for the last strand that has started in the simulator.
				A \a strand is a sequence of serially executed instructions 
				containing no parallel control.
	*/
	TIME last_strand_start_time;


	/**
	*    @var last_function_runtime
	*    @brief Duration of the last function that was called. 
	*/
	TIME last_function_runtime;

	/**
	*    @var total_locks_running
	*    @brief Tracks the current number of locks running, for calculation of 
				lock span. Lock span is only recorded when total_locks_running
				is 1 and a lock is then released. 
	*/
	int total_locks_running;

 private:
	// prevent generated functions --------------------------------------------
	ParasiteTool(const ParasiteTool&);
	ParasiteTool& operator=(const ParasiteTool&);
};

#endif  // PARASITE_TOOL_H_ 
