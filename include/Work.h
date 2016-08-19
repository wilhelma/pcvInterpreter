/**
 *
 *    @file  Work.h
 *   @brief  Declarations for the class `Work`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef WORK
#define WORK

#include <stdbool.h>
#include <iostream>
#include <limits>
#include <functional>
#include <unordered_map>
#include <string>

#include "CallSiteHashtable.h"
#include "CallSiteProfile.h"
#include "Types.h"
#include "Utility.h"

/**
*    @typedef call_site_work_hashtable_t
*    @brief std::unordered_map used as a hashtable for matching CALLSITE keys
			to call_site_work_profile_t values. 
*/
typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_work_profile_t>, CallSiteHash, CallSiteEqual> call_site_work_hashtable_t;

class Work {

 public:
	Work();
	explicit Work(Work const& hshtable);
	
	Work* operator=(const Work& rhs);
	~Work();

	void print();

	/**
	*    @fn clear()
	*    @brief Clears the unordered_map / hashtable. 
	*/
	void clear();

	std::shared_ptr<call_site_work_profile_t>  getProfile(CALLSITE call_site);

	void record_call_site(CALLSITE call_site, FUN_SG function_signature);

	/**
	*    @fn add_to_call_site(CALLSITE call_site, TIME work, bool is_local)
	*    @brief Adds work and span to the profile for
			    call_site contained in hashtable.
	*/
	void add_to_call_site(CALLSITE call_site, TIME work, bool is_local);

	TIME total;

	TIME operator()();

	/**
	*    @var hashtable
	*    @brief A std::shared_ptr to a call_site_work_hashtable_t that maps
				CALLSITE keys to call_site_work_profile_t values. 
	*/
	std::shared_ptr<call_site_work_hashtable_t> hashtable;
	
};

#endif  // WORK


