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
#include <stdlib.h>
#include <utility>
#include "Span.h"

Span::Span() : total(0) {
	hashtable = std::shared_ptr<call_site_span_hashtable_t>(new call_site_span_hashtable_t);
	total = static_cast<TIME>(0);
}

Span::Span(Span const& other_span) : total(0) {
	hashtable = std::move(other_span.hashtable);
	total = other_span.total;
}

Span::~Span() {

}

TIME Span::operator()() {
	return total;
}

std::shared_ptr<CallSiteSpanProfile> Span::profileAt(CALLSITE call_site) {

	if (!hashtable->count(call_site)) {
		std::cout << "WORK PROFILE NOT INITIALIZED CORRECTLY at call site " <<
			call_site << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return hashtable->at(call_site);
}


void Span::add(TIME local_work) {
	total = total + local_work;
}

void Span::add(Span* other_span) {

	total = total + other_span->total;
	for (auto const &it : *(other_span->hashtable)) {
		CALLSITE call_site = it.first;
		if (hashtable->count(call_site)) {
		    profileAt(call_site)->add_in_callsite_span(other_span->hashtable->at(call_site));
		    profileAt(call_site)->lock_wait_time = 
		    				other_span->hashtable->at(call_site)->lock_wait_time;
		} else  {
			std::pair<CALLSITE, std::shared_ptr<CallSiteSpanProfile> > 
                                  newPair(call_site, other_span->hashtable->at(call_site));
			hashtable->insert(newPair);
		}
	}
}

void Span::add_to_call_site(CALLSITE call_site, TIME span, TIME end_time) {
	if (!hashtable->count(call_site))
		init_call_site(call_site, end_time);
	profileAt(call_site)->span += span;
	profileAt(call_site)->start = std::min(profileAt(call_site)->start, end_time);
}

void Span::clear() {
	hashtable->clear();
	total = static_cast<TIME>(0);
}


void Span::init_call_site(CALLSITE call_site, TIME start_time) {

	std::shared_ptr<CallSiteSpanProfile> new_profile(new CallSiteSpanProfile());
	new_profile->init(call_site, start_time);
	std::pair<CALLSITE, std::shared_ptr<CallSiteSpanProfile>> 
                                    newPair(call_site, new_profile);
	hashtable->insert(newPair);
}


void Span::print() {

	for (auto const &it : *hashtable) {
		CALLSITE call_site = it.first;
		profileAt(call_site)->print();
	}
}

void Span::set(Span* other_span) {
	clear();
	hashtable = std::move(other_span->hashtable);
	total = other_span->total;
}

void Span::add_lock_wait_time(CALLSITE call_site,
							  TIME lock_wait_time) {

	profileAt(call_site)->lock_wait_time += lock_wait_time;
}




