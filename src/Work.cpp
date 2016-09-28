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




Work::Work() : total(0) {

	std::shared_ptr<call_site_work_hashtable_t> hshtable(new call_site_work_hashtable_t());
	hashtable = hshtable;
	total = static_cast<TIME>(0);
}

Work::Work(const Work& hashtable_object) : total(0) {
	hashtable = std::move(hashtable_object.hashtable);
	total = hashtable_object.total;
}


Work::~Work() {

}

TIME Work::operator()(){
	return total;
}

std::shared_ptr<CallSiteWorkProfile>  Work::profileAt(CALLSITE call_site) {

	if (!hashtable->count(call_site)) {
		std::cout << "WORK PROFILE NOT INITIALIZED CORRECTLY at call site " <<
			call_site << std::endl;
	}
	return hashtable->at(call_site);
}

void Work::print() {

	for (auto const &it : *hashtable) {
		CALLSITE key = it.first;
		profileAt(key)->print();
	}
}

void Work::clear() {
	hashtable->clear();
}

void Work::init_call_site(CALLSITE call_site, FUN_SG function_signature) {

	if (!hashtable->count(call_site)) {
		std::shared_ptr<CallSiteWorkProfile> new_ptr(new CallSiteWorkProfile());
		new_ptr->init_callsite_profile(call_site, function_signature);
		std::pair<CALLSITE, std::shared_ptr<CallSiteWorkProfile>> 
                                    newPair(call_site, new_ptr);
		hashtable->insert(newPair);
	}
}


void Work::add(TIME local_work) {
	total += local_work;
}


void Work::add_to_call_site(CALLSITE call_site, FUN_SG function_signature,
						    TIME work) {
	if (!hashtable->count(call_site))
		init_call_site(call_site, function_signature);
	profileAt(call_site)->work += work;
}

TIME Work::at_call_site(CALLSITE call_site) {
	return profileAt(call_site)->work;
}

void Work::record_call_site(CALLSITE call_site, FUN_SG function_signature) {

	if (!hashtable->count(call_site)) {
		init_call_site(call_site, function_signature);
	}

	profileAt(call_site)->count += 1;
}


