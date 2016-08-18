/**
 *
 *    @file  CallSiteHashtable.h
 *   @brief  General call site structs
 *
 *    @date  07/29/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_HASHTABLE_H_
#define CALL_SITE_HASHTABLE_H_

#include "Types.h"

/**
*    @struct CallSiteHash
*    @brief Struct providing the CALLSITE hash operator needed for std::unordered_map
			used for call_site_work_hashtable_t and call_site_span_hashtable_t.
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
			used for call_site_work_hashtable_t and call_site_span_hashtable_t. 
*/
struct CallSiteEqual {
	bool operator()(const CALLSITE& lhs, const CALLSITE& rhs) const {
		return lhs == rhs;
	}
};

#endif  // CALL_SITE_HASHTABLE_H_
