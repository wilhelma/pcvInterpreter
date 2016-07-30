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

void CallSiteWorkHashtable::increment_count(CALLSITE call_site) {
	if (hashtable->count(call_site)) {
		CallSiteWorkProfile profile(hashtable->at(call_site));
  		profile.prof->count += 1;
	} else {
		std::shared_ptr<call_site_work_profile_t> new_ptr(new call_site_work_profile_t());
		CallSiteWorkProfile new_profile(new_ptr);
		new_profile.init_callsite_profile(call_site, static_cast<TIME>(0));
		std::pair<CALLSITE, std::shared_ptr<call_site_work_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}


void CallSiteWorkHashtable::add_work(CALLSITE call_site, TIME work) {
	if (hashtable->count(call_site)) {
		CallSiteWorkProfile profile(hashtable->at(call_site));
  		profile.prof->work += work;
	} else {
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




