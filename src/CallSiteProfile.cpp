/**
 *
 *    @file  CallSiteProfile.cpp
 *   @brief  Implementation file for the class `CallSiteProfile`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "CallSiteProfile.h"

CallSiteProfile::~CallSiteProfile() {}

CallSiteProfile::CallSiteProfile(std::shared_ptr<call_site_profile_t> init_profile) {
	prof = init_profile;
}

void CallSiteProfile::add_in_callsite_profile_entries(const std::shared_ptr<call_site_profile_t> profile_to_add) {
	prof->work += profile_to_add->work;
	prof->span += profile_to_add->span;
	prof->count += profile_to_add->count;
	prof->parallelism = prof->work/prof->span;
}

void CallSiteProfile::init_callsite_profile(CALLSITE call_site,
                                            TIME work, TIME span) {
	prof->call_site = call_site;
	prof->work = work;
	prof->span = span;
	prof->parallelism = work/span;
}


