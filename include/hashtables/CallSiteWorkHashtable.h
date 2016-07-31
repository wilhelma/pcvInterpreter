/**
 *
 *    @file  CallSiteWorkHashtable.h
 *   @brief  Declarations for the class `CallSiteWorkHashtable`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_WORK_HASHTABLE_H_
#define CALL_SITE_WORK_HASHTABLE_H_

#include <stdbool.h>
#include <iostream>
#include <limits>
#include <functional>
#include <unordered_map>
#include <string>

#include "CallSiteHashtable.h"
#include "CallSiteProfile.h"
#include "Types.h"

/**
*    @typedef call_site_work_hashtable_t
*    @brief std::unordered_map used as a hashtable for matching CALLSITE keys
			to call_site_work_profile_t values. 
*/
typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_work_profile_t>, CallSiteHash, CallSiteEqual> call_site_work_hashtable_t;

class CallSiteWorkHashtable {

 public:
	CallSiteWorkHashtable();
	explicit CallSiteWorkHashtable(CallSiteWorkHashtable const& hshtable);
	
	CallSiteWorkHashtable* operator=(const CallSiteWorkHashtable& rhs);
	~CallSiteWorkHashtable();

	void print();

	/**
	*    @fn contains()
	*    @brief Returns true if the hashtable contains a profile for call_site.
	*/
	bool contains(CALLSITE call_site);


	/**
	*    @fn clear()
	*    @brief Clears the unordered_map / hashtable. 
	*/
	void clear();

	void increment_count(CALLSITE call_site,  FUN_SG function_signature);

	/**
	*    @fn add_data(...)
	*    @brief Adds work and span to the profile for
			    call_site contained in hashtable.
	*/
	void add_work(CALLSITE call_site, FUN_SG function_signature, TIME work);

	/**
	*    @var hashtable
	*    @brief A std::shared_ptr to a call_site_work_hashtable_t that maps
				CALLSITE keys to call_site_work_profile_t values. 
	*/
	std::shared_ptr<call_site_work_hashtable_t> hashtable;
	
};

#endif  // CALL_SITE_WORK_HASHTABLE_H_


