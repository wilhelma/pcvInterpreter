/*
 * CallSiteHashtable.h
 *
 * Created on: June 16, 2016
 *      Author: knapp
 */


#ifndef CALL_SITE_HASHTABLE_H_
#define CALL_SITE_HASHTABLE_H_

#include <stdbool.h>
#include <iostream>
#include <limits>
#include <functional>
#include <unordered_map>
#include <string>


#include "Types.h"
#include "CallSiteProfile.h"
#include "EndProfiles.h"

struct CallSiteHash {
	std::size_t operator()(const CALLSITE& k) const {
		std::hash<int> int_hash;
		return int_hash(k);
	}
};
 
struct CallSiteEqual {
	bool operator()(const CALLSITE& lhs, const CALLSITE& rhs) const {
		return lhs == rhs;
	}
};

typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_profile_t>, CallSiteHash, CallSiteEqual> call_site_hashtable_t;
typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_end_profile_t>, CallSiteHash, CallSiteEqual>  call_site_end_hashtable_t;

class CallSiteHashtable {
 public:
	CallSiteHashtable();
	explicit CallSiteHashtable(std::shared_ptr<call_site_hashtable_t> hshtable);
	~CallSiteHashtable();

	// adds in hashtable into right hashtable
	void add_in_hashtable(const std::shared_ptr<call_site_hashtable_t> 
														  added_hashtable);

	// adds work, span, local_work, and local_span tto profile for call_site
	void add_data_to_hashtable(bool is_top_function,
	                         CALLSITE call_site,
	                         double work, double span,
	                         double local_work, double local_span);

	// adds local work and span to profile for call_site
	void add_local_data_to_hashtable(CALLSITE call_site,
	                                 double local_work, double local_span);

	std::shared_ptr<call_site_hashtable_t> hashtable;

 private:
	CallSiteHashtable(const CallSiteHashtable&);
	CallSiteHashtable& operator=(const CallSiteHashtable&);
};

#endif  // CALL_SITE_HASHTABLE_H_


