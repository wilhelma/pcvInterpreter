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

void CallSiteSpanProfile::add_in_callsite_span_profile_entries(const std::shared_ptr<call_site_span_profile_t> profile_to_add) {
	prof->span += profile_to_add->span;
}

void CallSiteSpanProfile::init_callsite_span_profile(CALLSITE call_site, TIME span) {
	prof->call_site = call_site;
	prof->span = span;
}

void CallSiteSpanProfile::print() {
	printf(" ================ \n");
	printf("CALL SITE %d \n", static_cast<int>(prof->call_site));
	printf("SPAN is %llu \n", static_cast<unsigned long long>(prof->span));
	printf("================\n");
}


