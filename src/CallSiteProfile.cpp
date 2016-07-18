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
  prof->local_work += profile_to_add->local_work;
  prof->local_span += profile_to_add->local_span;
  prof->local_count += profile_to_add->local_count;
  prof->work += profile_to_add->work;
  prof->span += profile_to_add->span;
  prof->count += profile_to_add->count;
  prof->top_work += profile_to_add->top_work;
  prof->top_span += profile_to_add->top_span;
  prof->top_count += profile_to_add->top_count;
}

void CallSiteProfile::init_callsite_profile(CALLSITE call_site,
        						                         bool is_top_function,
                                             uint64_t work, uint64_t span,
                                             uint64_t local_work, uint64_t local_span) {
    prof->call_site = call_site;
    prof->local_count = 0;
    prof->local_work = local_work;
    prof->local_span = local_span;

    prof->count = 1;

    if (is_top_function)
        prof->top_count = 1;
    else
        prof->top_count = 0;

    prof->work = work;
    prof->span = span;
    prof->top_work = work;
    prof->top_span = span;
}


