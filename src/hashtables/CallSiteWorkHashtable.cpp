/**
 *
 *    @file  CallSiteWorkHashtable.cpp
 *   @brief  Implementation file for the class `CallSiteWorkHashtable`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include <utility>
#include "CallSiteWorkHashtable.h"

CallSiteWorkHashtable::CallSiteWorkHashtable() {

	std::shared_ptr<call_site_work_hashtable_t> hshtable(new call_site_work_hashtable_t());
	hashtable = hshtable;
}

CallSiteWorkHashtable::CallSiteWorkHashtable(CallSiteWorkHashtable const& hashtable_object) {
	hashtable = std::move(hashtable_object.hashtable);
}


CallSiteWorkHashtable::~CallSiteWorkHashtable() {

}

void CallSiteWorkHashtable::print() {

	for (auto const &it : *hashtable) {
		CALLSITE key = it.first;
		CallSiteWorkProfile currentProfile(hashtable->at(key));
		currentProfile.print();
	}
}

void CallSiteWorkHashtable::clear() {
	hashtable->clear();
}

void CallSiteWorkHashtable::add(CallSiteWorkHashtable* hashtable_object) {
	for (auto const &it : *(hashtable_object->hashtable)) {
		CALLSITE key = it.first;

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable->count(key)) {
			CallSiteWorkProfile existingProfile(hashtable->at(key));
		    existingProfile.add_in_callsite_profile_entries(hashtable_object->hashtable->at(key));
		} else  {
			std::pair<CALLSITE, std::shared_ptr<call_site_work_profile_t> > 
                                       newPair(key, hashtable_object->hashtable->at(key));
			hashtable->insert(newPair);
		}
	}
}

void CallSiteWorkHashtable::add_data(CALLSITE call_site, TIME work) {
	printf("HERE 0 \n");
	if (hashtable->count(call_site)) {
		printf("HERE 1 \n");
		CallSiteWorkProfile profile(hashtable->at(call_site));
  		profile.prof->count += 1;
  		profile.prof->work += work;
	} else {
		printf("HERE 2 \n");
		std::shared_ptr<call_site_work_profile_t> new_ptr(new call_site_work_profile_t());
		CallSiteWorkProfile new_profile(new_ptr);
		new_profile.init_callsite_profile(call_site, work);
		std::pair<CALLSITE, std::shared_ptr<call_site_work_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}

bool CallSiteWorkHashtable::contains(CALLSITE call_site) {
	if (hashtable->count(call_site))
		return true;
	else 
		return false;
}




