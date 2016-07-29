/**
 *
 *    @file  CallSiteSpanHashtable.cpp
 *   @brief  Implementation file for the class `CallSiteSpanHashtable`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include <utility>
#include "CallSiteSpanHashtable.h"

CallSiteSpanHashtable::CallSiteSpanHashtable() {
	hashtable = std::shared_ptr<call_site_span_hashtable_t>(new call_site_span_hashtable_t);
}

CallSiteSpanHashtable::CallSiteSpanHashtable(CallSiteSpanHashtable const& hashtable_object) {
	hashtable = std::move(hashtable_object.hashtable);
}


CallSiteSpanHashtable::~CallSiteSpanHashtable() {

}

void CallSiteSpanHashtable::print() {

	for (auto const &it : *hashtable) {
		CALLSITE key = it.first;
		CallSiteSpanProfile currentProfile(hashtable->at(key));
		currentProfile.print();
	}
}

void CallSiteSpanHashtable::clear() {
	hashtable->clear();
}

void CallSiteSpanHashtable::add(CallSiteSpanHashtable* hashtable_object) {
	for (auto const &it : *(hashtable_object->hashtable)) {
		CALLSITE key = it.first;

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable->count(key)) {
			CallSiteSpanProfile existingProfile(hashtable->at(key));
		    existingProfile.add_in_callsite_span_profile_entries(hashtable_object->hashtable->at(key));
		} else  {
			std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t> > 
                                       newPair(key, hashtable_object->hashtable->at(key));
			hashtable->insert(newPair);
		}
	}
}

void CallSiteSpanHashtable::add_data(CALLSITE call_site, TIME span) {
	if (hashtable->count(call_site)) {
		CallSiteSpanProfile profile(hashtable->at(call_site));
  		profile.prof->span += span;
	} else {

		std::shared_ptr<call_site_span_profile_t> new_ptr(new call_site_span_profile_t());
		CallSiteSpanProfile new_profile(new_ptr);
		new_profile.init_callsite_span_profile(call_site, span);
		std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}

bool CallSiteSpanHashtable::contains(CALLSITE call_site) {
	if (hashtable->count(call_site))
		return true;
	else 
		return false;
}




