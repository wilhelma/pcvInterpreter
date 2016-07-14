
#include "CallSiteEndProfile.h"

/**
*    @fn getEndCallSiteWorkProfile(std::shared_ptr<call_site_profile_t>  collected_profile, 
                                   std::shared_ptr<call_site_end_profile_t> end_profile)
*    @brief Converts work and span information in collected_profile to more detailed work 
            information contained in end profile. 
*/           
void CallSiteEndProfile::getEndCallSiteWorkProfile(std::shared_ptr<call_site_profile_t> collected_profile) {
  // work excluding recurisve calls
  work = collected_profile->work;
  span = collected_profile->span;
  parallelism = work/span;
  count = collected_profile->count;

  // work data from top calls of call site
  top_work = collected_profile->top_work;
  top_span = collected_profile->top_span;
  top_parallelism = top_work/top_span;
  top_count = collected_profile->top_count;

  // local work call site data
  local_work = collected_profile->local_work;
  local_span = collected_profile->local_span;
  local_parallelism = top_work/top_span;
  local_count = collected_profile->top_count;
}

/**
*    @fn getEndCallSiteSpanProfile(std::shared_ptr<call_site_profile_t>  collected_profile, 
                                   std::shared_ptr<call_site_end_profile_t> end_profile)
*    @brief Converts on_span information in collected_profile to more detailed 
            on_span information contained in end_profile. The on_span information is work 
            and span data from invocations on the critical path. 
*/
void CallSiteEndProfile::getEndCallSiteSpanProfile(std::shared_ptr<call_site_profile_t> collected_profile) {
  // span data excluding recursive calls
  work_on_span = collected_profile->work;
  span_on_span = collected_profile->span;
  parallelism_on_span = work_on_span/ 
                                     span_on_span;
  count_on_span = collected_profile->count;

  // data from top calls of call site
  top_work_on_span = collected_profile->top_work;
  top_span_on_span = collected_profile->top_span;
  top_parallelism_on_span = top_work_on_span
                                    / top_span_on_span;
  top_count_on_span = collected_profile->top_count;

  // local(?) span call site data
  local_work_on_span = collected_profile->local_work;
  local_span_on_span = collected_profile->local_span;
  local_parallelism_on_span = local_work_on_span / 
                                           local_span_on_span;
  local_count_on_span = collected_profile->local_count;
}

CallSiteEndProfile::CallSiteEndProfile(std::shared_ptr<call_site_profile_t> collected_profile) : call_site(0) {

  call_site = collected_profile->call_site;
  getEndCallSiteWorkProfile(collected_profile);
  getEndCallSiteSpanProfile(collected_profile);
}

CallSiteEndProfile::CallSiteEndProfile() : call_site(0) {}


CallSiteEndProfile::~CallSiteEndProfile() {

}