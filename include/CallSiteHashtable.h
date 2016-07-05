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
#include "EndProfiles.h"
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
			to call_site_end_profile_t values. 
*/
typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_end_profile_t>, CallSiteHash, CallSiteEqual>  call_site_end_hashtable_t;

class CallSiteHashtable {
 public:

	CallSiteHashtable();
	explicit CallSiteHashtable(std::shared_ptr<call_site_hashtable_t> hshtable);
	~CallSiteHashtable();


	/**
	*    @fn call_site_end_hashtable_t
	*    @brief Adds in added_hashtable into the class's hashtable.
	*/
	void add_in_hashtable(const std::shared_ptr<call_site_hashtable_t> 
														  added_hashtable);

	/**
	*    @fn call_site_end_hashtable_t
	*    @brief Adds work, span, local_work, and local_span to the profile for
			    call_site contained in hashtable.
	*/
	void add_data_to_hashtable(bool is_top_function,
	                         CALLSITE call_site,
	                         double work, double span,
	                         double local_work, double local_span);

	/**
	*    @fn call_site_end_hashtable_t
	*    @brief Adds local_work and local_span to the profile for
			    call_site contained in hashtable.
	*/
	void add_local_data_to_hashtable(CALLSITE call_site,
	                                 double local_work, double local_span);

	/**
	*    @var hashtable
	*    @brief A std::shared_ptr to a call_site_hashtable_t that maps
				CALLSITE keys to call_site_profile_t values. 
	*/
	std::shared_ptr<call_site_hashtable_t> hashtable;

 private:
	CallSiteHashtable(const CallSiteHashtable&);
	CallSiteHashtable& operator=(const CallSiteHashtable&);
};

#endif  // CALL_SITE_HASHTABLE_H_


