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
	total = 0;
}

Work::Work(Work const& hashtable_object) {
	hashtable = std::move(hashtable_object.hashtable);
	total = hashtable_object.total;
}


Work::~Work() {

}

TIME Work::operator()(){
	return total;
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

std::shared_ptr<call_site_work_profile_t>  Work::getProfile(CALLSITE call_site) {

	try {
		if (hashtable->count(call_site)) {
			return hashtable->at(call_site);
		} else {
			throw CallsiteNotInitializedException();
		}
	}
	catch(CallsiteNotInitializedException& e) {
		std::cout << "work " << e.what() << std::endl;
	}
}

void Work::add_to_call_site(CALLSITE call_site, TIME work, bool is_local) {

	if (is_local)
		total += work;
	CallSiteWorkProfile profile(getProfile(call_site));
	profile.prof->work += work;
}

void Work::record_call_site(CALLSITE call_site, FUN_SG function_signature) {

	if (hashtable->count(call_site)) {
		CallSiteWorkProfile profile(hashtable->at(call_site));
		profile.prof->count += 1;
		return;
	}

	std::shared_ptr<call_site_work_profile_t> new_ptr(new call_site_work_profile_t());
	CallSiteWorkProfile new_profile(new_ptr);
	new_profile.init_callsite_profile(call_site, function_signature);
	std::pair<CALLSITE, std::shared_ptr<call_site_work_profile_t>> 
                                    newPair(call_site, new_profile.prof);
	hashtable->insert(newPair);
}


