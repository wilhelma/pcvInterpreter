/**
 *
 *    @file  CallSiteHashtable.h
 *   @brief  Declarations for the class `CallSiteHashtable`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_HASHTABLE_H_
#define CALL_SITE_HASHTABLE_H_

#include <stdbool.h>
#include <iostream>
#include <limits>
#include <functional>
#include <unordered_map>
#include <string>

#include "CallSiteProfile.h"
#include "Types.h"

/**
*    @struct CallSiteHash
*    @brief Struct providing the CALLSITE hash operator needed for std::unordered_map
			used for call_site_hashtable_t and call_site_end_hashtable_t.  
*/
struct CallSiteHash {
	std::size_t operator()(const CALLSITE& k) const {
		std::hash<int> int_hash;
		return int_hash(k);
	}
};
 

/**
*    @struct CallSiteEqual
*    @brief Struct providing the CALLSITE hash operator needed for std::unordered_map
			used for call_site_end_hashtable_t and call_site_end_hashtable_t. 
*/
struct CallSiteEqual {
	bool operator()(const CALLSITE& lhs, const CALLSITE& rhs) const {
		return lhs == rhs;
	}
};

/**
*    @typedef call_site_hashtable_t
*    @brief std::unordered_map used as a hashtable for matching CALLSITE keys
			to call_site_profile_t values. 
*/
typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_profile_t>, CallSiteHash, CallSiteEqual> call_site_hashtable_t;

/**
*    @typedef call_site_end_hashtable_t
*    @brief std::unordered_map used as a hashtable for matching CALLSITE keys
			to CallSiteEndProfile values. 
*/
typedef std::unordered_map<CALLSITE, std::shared_ptr<CallSiteProfile>, CallSiteHash, CallSiteEqual>  call_site_end_hashtable_t;

class CallSiteHashtable {

 public:
	CallSiteHashtable();
	explicit CallSiteHashtable(CallSiteHashtable const& hshtable);
	
	CallSiteHashtable* operator=(const CallSiteHashtable& rhs);
	~CallSiteHashtable();


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

	/**
	*    @fn add_in_hashtable(...)
	*    @brief Adds in added_hashtable into the class's hashtable.
	*/
	void add(CallSiteHashtable* hashtable_object);

	/**
	*    @fn add_data(...)
	*    @brief Adds work and span to the profile for
			    call_site contained in hashtable.
	*/
	void add_data(CALLSITE call_site, TIME work, TIME span);

	/**
	*    @var hashtable
	*    @brief A std::shared_ptr to a call_site_hashtable_t that maps
				CALLSITE keys to call_site_profile_t values. 
	*/
	std::shared_ptr<call_site_hashtable_t> hashtable;
	
};

#endif  // CALL_SITE_HASHTABLE_H_


