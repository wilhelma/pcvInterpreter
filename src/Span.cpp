/**
 *
 *    @file  Span.cpp
 *   @brief  Implementation file for the class `Span`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <cassert>
#include <utility>
#include "Span.h"

Span::Span() {
	hashtable = std::shared_ptr<call_site_span_hashtable_t>(new call_site_span_hashtable_t);
	total = 0;
}

Span::Span(Span const& other_span) {
	hashtable = std::move(other_span.hashtable);
	total = other_span.total;
}

Span::~Span() {

}

TIME Span::operator()() {
	return total;
}

void Span::print() {

	for (auto const &it : *hashtable) {
		CALLSITE key = it.first;
		CallSiteSpanProfile currentProfile(hashtable->at(key));
		currentProfile.print();
	}
}

void Span::clear() {
	hashtable->clear();
	total = 0;
}

void Span::set(Span* other_span) {
	clear();
	hashtable = std::move(other_span->hashtable);
	total = other_span->total;
}

void Span::add(Span* other_span) {

	total += other_span->total;
	for (auto const &it : *(other_span->hashtable)) {
		CALLSITE key = it.first;

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable->count(key)) {
			CallSiteSpanProfile existingProfile(hashtable->at(key));
		    existingProfile.add_in_callsite_span(other_span->hashtable->at(key));
		    existingProfile.prof->lock_wait_time = 
		    				other_span->hashtable->at(key)->lock_wait_time;
		} else  {
			std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t> > 
                                       newPair(key, other_span->hashtable->at(key));
			hashtable->insert(newPair);
		}
	}
}

void Span::set_lock_wait_time(CALLSITE call_site,
							  TIME lock_wait_time) {

	if (hashtable->count(call_site)) {
		CallSiteSpanProfile profile(hashtable->at(call_site));
  	    profile.prof->lock_wait_time = lock_wait_time;
	} else {
	    std::shared_ptr<call_site_span_profile_t> new_ptr(new call_site_span_profile_t());
		CallSiteSpanProfile new_profile(new_ptr);
		new_profile.init_callsite_span(call_site, static_cast<TIME>(0));
	    new_profile.prof->lock_wait_time = lock_wait_time;
		std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}


void Span::add_to_call_site(CALLSITE call_site,
								     TIME span) {

	if (hashtable->count(call_site)) {
		CallSiteSpanProfile profile(hashtable->at(call_site));
  		profile.prof->span += span;
	} else {
		std::shared_ptr<call_site_span_profile_t> new_ptr(new call_site_span_profile_t());
		CallSiteSpanProfile new_profile(new_ptr);
		new_profile.init_callsite_span(call_site, span);
		std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t>> 
                                        newPair(call_site, new_profile.prof);
		hashtable->insert(newPair);
	}
}

bool Span::contains(CALLSITE call_site) {
	if (hashtable->count(call_site))
		return true;
	else 
		return false;
}



