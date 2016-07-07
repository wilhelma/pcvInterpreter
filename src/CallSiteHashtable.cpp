/**
 *
 *    @file  CallSiteHashtable.cpp
 *   @brief  Implementation file for the class `CallSiteHashtable`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include <utility>
#include "CallSiteHashtable.h"


CallSiteHashtable::~CallSiteHashtable() {}

CallSiteHashtable::CallSiteHashtable(std::shared_ptr<call_site_hashtable_t> hshtable) {
	hashtable = hshtable;
}

void CallSiteHashtable::add_in_hashtable( const std::shared_ptr<call_site_hashtable_t> hashtable_to_add) {

	for (auto const &it : *hashtable_to_add) {
		CALLSITE key = it.first;

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable->count(key)) {
			CallSiteProfile existingProfile(hashtable->at(key));
		    existingProfile.add_in_callsite_profile_entries(hashtable_to_add->at(key));
		} else  {
			std::pair<CALLSITE, std::shared_ptr<call_site_profile_t> > 
                                       newPair(key, hashtable_to_add->at(key));
			hashtable->insert(newPair);
		}
	}
}

void CallSiteHashtable::add_data_to_hashtable(bool is_top_function,
                         					  CALLSITE call_site,
                         					  double work, double span,
                         					  double local_work, double local_span) {
	if (hashtable->count(call_site)) {
		CallSiteProfile profile(hashtable->at(call_site));
		profile.prof->local_count += 1;
  		profile.prof->local_work += local_work;
  		profile.prof->local_span += local_span;
  	
  		profile.prof->count += 1;
  		profile.prof->work += work;
  		profile.prof->span += span;

  		if (is_top_function) {
			profile.prof->top_count += 1;
			profile.prof->top_work += work;
  			profile.prof->top_span += span;
  		}
	} else {

		std::shared_ptr<call_site_profile_t> new_ptr(new call_site_profile_t());
		CallSiteProfile new_profile(new_ptr);
		new_profile.init_callsite_profile(call_site, is_top_function, 
                                          work, span, local_work, local_span);
		std::pair<CALLSITE, std::shared_ptr<call_site_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}

void CallSiteHashtable::add_local_data_to_hashtable(CALLSITE call_site,
                            		    			double local_work, double local_span) {
	if (hashtable->count(call_site)) {
		CallSiteProfile profile(hashtable->at(call_site));
		profile.prof->local_count += 1;
  		profile.prof->local_work += local_work;
  		profile.prof->local_span += local_span;
	} else {
		// THROW EXCEPTION
		std::cout << "ERROR: cannot add local values to nonexistent hashtable \
                      entry" << std::endl;
	}
}


