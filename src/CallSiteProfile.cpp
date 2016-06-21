#include "CallSiteProfile.h"


std::unique_ptr<call_site_profile_t> CallSiteProfile::create_callsite_profile(CALLSITE call_site,
						                         bool is_top_function,
                                     double work, double span,
                                     double local_work, double local_span) {


	std::unique_ptr<call_site_profile_t> new_profile(new call_site_profile_t());

	new_profile->call_site = call_site;
	new_profile->local_count = 0;
  new_profile->local_work = local_work;
  new_profile->local_span = local_span;

  new_profile->count = 1;

  if (is_top_function)
    new_profile->top_count = 1;
  else
    new_profile->top_count = 0;

  new_profile->work = work;
  new_profile->span = span;
  new_profile->top_work = work;
  new_profile->top_span = span;

  return new_profile;
}


// add entries in profile_to_add to entries in profile
void CallSiteProfile::combine_callsite_profile_entries(const std::unique_ptr<call_site_profile_t> profile_to_add,
									                    std::unique_ptr<call_site_profile_t>  profile) {

  profile->local_work += profile_to_add->local_work;
  profile->local_span += profile_to_add->local_span;
  profile->local_count += profile_to_add->local_count;
  profile->work += profile_to_add->work;
  profile->span += profile_to_add->span;
  profile->count += profile_to_add->count;
  profile->top_work += profile_to_add->top_work;
  profile->top_span += profile_to_add->top_span;
  profile->top_count += profile_to_add->top_count;
}