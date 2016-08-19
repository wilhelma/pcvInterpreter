/**
 *
 *    @file  CallSiteSpanProfile.cpp
 *   @brief  Implementation file for the class `CallSiteSpanProfile`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "CallSiteSpanProfile.h"

CallSiteSpanProfile::~CallSiteSpanProfile() {}

CallSiteSpanProfile::CallSiteSpanProfile(std::shared_ptr<call_site_span_profile_t> init_profile) {
	prof = init_profile;
}

void CallSiteSpanProfile::add_in_callsite_span
			  (const std::shared_ptr<call_site_span_profile_t> profile_to_add) {
	prof->span += profile_to_add->span;
	prof->start = std::min(prof->start, profile_to_add->start);
	prof->stop = std::max(prof->stop, profile_to_add->stop);
}

void CallSiteSpanProfile::init(CALLSITE call_site) {
	prof->call_site = call_site;
}

void CallSiteSpanProfile::print() {
	printf(" ================ \n");
	printf("CALL SITE %d \n", static_cast<int>(prof->call_site));
	printf("SPAN is %llu \n", static_cast<unsigned long long>(prof->span));
	printf("LOCK WAIT TIME is %llu \n", 
					     static_cast<unsigned long long>(prof->lock_wait_time));
	printf("LOCK SPAN IS %f of SPAN \n", 
									static_cast<double>(prof->lock_wait_time) / 
								    static_cast<double>(prof->span));
	printf("================\n");
}