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

void Span::add(Span* other_span) {

	total += other_span->total;
	for (auto const &it : *(other_span->hashtable)) {
		CALLSITE call_site = it.first;
		if (hashtable->count(call_site)) {
			CallSiteSpanProfile existingProfile(hashtable->at(call_site));
		    existingProfile.add_in_callsite_span(other_span->hashtable->at(call_site));
		    existingProfile.prof->lock_wait_time = 
		    				other_span->hashtable->at(call_site)->lock_wait_time;
		} else  {
			std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t> > 
                                  newPair(call_site, other_span->hashtable->at(call_site));
			hashtable->insert(newPair);
		}
	}
}

void Span::add_to_call_site(CALLSITE call_site, TIME span, TIME end_time) {

	total += span;
	if (!hashtable->count(call_site))
		init_call_site(call_site, end_time);
	CallSiteSpanProfile profile(hashtable->at(call_site));
	profile.prof->span += span;
	profile.prof->start = std::min(profile.prof->start, end_time);
	profile.prof->stop = std::max(profile.prof->stop, end_time);
}

void Span::clear() {
	hashtable->clear();
	total = 0;
}


void Span::init_call_site(CALLSITE call_site, TIME start_time) {

	std::shared_ptr<call_site_span_profile_t> new_ptr(new call_site_span_profile_t());
	CallSiteSpanProfile new_profile(new_ptr);
	new_profile.init(call_site);
	std::pair<CALLSITE, std::shared_ptr<call_site_span_profile_t>> 
                                    newPair(call_site, new_profile.prof);
	hashtable->insert(newPair);
}


void Span::print() {

	for (auto const &it : *hashtable) {
		CALLSITE call_site = it.first;
		CallSiteSpanProfile currentProfile(hashtable->at(call_site));
		currentProfile.print();
	}
}

void Span::set(Span* other_span) {
	clear();
	hashtable = std::move(other_span->hashtable);
	total = other_span->total;
}

void Span::set_lock_wait_time(CALLSITE call_site,
							  TIME lock_wait_time) {

	CallSiteSpanProfile profile(hashtable->at(call_site));
	profile.prof->lock_wait_time = lock_wait_time;
}




