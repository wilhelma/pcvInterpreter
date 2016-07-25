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

CallSiteHashtable::CallSiteHashtable() {
	hashtable = std::shared_ptr<call_site_hashtable_t>(new call_site_hashtable_t);
}

CallSiteHashtable::CallSiteHashtable(CallSiteHashtable const& hashtable_object) {
	hashtable = std::move(hashtable_object.hashtable);
}


CallSiteHashtable::~CallSiteHashtable() {

}

CallSiteHashtable* CallSiteHashtable::operator=(const CallSiteHashtable& table_object) {
	hashtable = table_object.hashtable;
	return this;
}

void CallSiteHashtable::print() {

	for (auto const &it : *hashtable) {
		CALLSITE key = it.first;
		CallSiteProfile currentProfile(hashtable->at(key));
		currentProfile.print();
	}
}

void CallSiteHashtable::clear() {
	hashtable->clear();
}

void CallSiteHashtable::add(CallSiteHashtable* hashtable_object) {
	for (auto const &it : *(hashtable_object->hashtable)) {
		CALLSITE key = it.first;

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable->count(key)) {
			CallSiteProfile existingProfile(hashtable->at(key));
		    existingProfile.add_in_callsite_profile_entries(hashtable_object->hashtable->at(key));
		} else  {
			std::pair<CALLSITE, std::shared_ptr<call_site_profile_t> > 
                                       newPair(key, hashtable_object->hashtable->at(key));
			hashtable->insert(newPair);
		}
	}
}

void CallSiteHashtable::add_data(CALLSITE call_site, TIME work, TIME span) {
	if (hashtable->count(call_site)) {
		CallSiteProfile profile(hashtable->at(call_site));
  	
  		profile.prof->count += 1;
  		profile.prof->work += work;
  		profile.prof->span += span;
	} else {

		std::shared_ptr<call_site_profile_t> new_ptr(new call_site_profile_t());
		CallSiteProfile new_profile(new_ptr);
		new_profile.init_callsite_profile(call_site, work, span);
		std::pair<CALLSITE, std::shared_ptr<call_site_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}

bool CallSiteHashtable::contains(CALLSITE call_site) {
	if (hashtable->count(call_site))
		return true;
	else 
		return false;
}




