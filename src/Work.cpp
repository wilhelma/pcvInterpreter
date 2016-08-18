/**
 *
 *    @file  Work.cpp
 *   @brief  Implementation file for the class `Work`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include <utility>
#include "Work.h"

Work::Work() {

	std::shared_ptr<call_site_work_hashtable_t> hshtable(new call_site_work_hashtable_t());
	hashtable = hshtable;
}

Work::Work(Work const& hashtable_object) {
	hashtable = std::move(hashtable_object.hashtable);
}


Work::~Work() {

}

void Work::print() {

	for (auto const &it : *hashtable) {
		CALLSITE key = it.first;
		CallSiteWorkProfile currentProfile(hashtable->at(key));
		currentProfile.print();
	}
}

void Work::clear() {
	hashtable->clear();
}

void Work::increment_count(CALLSITE call_site,
											FUN_SG function_signature) {
	if (hashtable->count(call_site)) {
		CallSiteWorkProfile profile(hashtable->at(call_site));
  		profile.prof->count += 1;
	} else {
		std::shared_ptr<call_site_work_profile_t> new_ptr(new call_site_work_profile_t());
		CallSiteWorkProfile new_profile(new_ptr);
		new_profile.init_callsite_profile(call_site, function_signature, static_cast<TIME>(0));
		std::pair<CALLSITE, std::shared_ptr<call_site_work_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}


void Work::add_to_call_site(CALLSITE call_site, 
									 FUN_SG function_signature,
									 TIME work) {
	total += work;
	if (hashtable->count(call_site)) {
		CallSiteWorkProfile profile(hashtable->at(call_site));
  		profile.prof->work += work;
	} else {
		std::shared_ptr<call_site_work_profile_t> new_ptr(new call_site_work_profile_t());
		CallSiteWorkProfile new_profile(new_ptr);
		new_profile.init_callsite_profile(call_site, function_signature, work);
		std::pair<CALLSITE, std::shared_ptr<call_site_work_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}

bool Work::contains(CALLSITE call_site) {
	if (hashtable->count(call_site))
		return true;
	else 
		return false;
}




