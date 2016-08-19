/**
 *
 *    @file  CallSiteWorkProfile.cpp
 *   @brief  Implementation file for the class `CallSiteWorkProfile`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "CallSiteWorkProfile.h"

CallSiteWorkProfile::~CallSiteWorkProfile() {}

CallSiteWorkProfile::CallSiteWorkProfile(std::shared_ptr<call_site_work_profile_t> init_profile) {
	prof = init_profile;
}

void CallSiteWorkProfile::add_in_callsite_profile_entries(const std::shared_ptr<call_site_work_profile_t> profile_to_add) {
	prof->work += profile_to_add->work;
	prof->count += (profile_to_add->count - 1);
}

void CallSiteWorkProfile::init_callsite_profile(CALLSITE call_site, FUN_SG function_signature) {
	prof->call_site = call_site;
	prof->function_signature = function_signature;
	prof->work = 0;
	prof->count = 1;
}

void CallSiteWorkProfile::print() {
	printf(" ================ \n");
	printf("CALL SITE %d \n", static_cast<int>(prof->call_site));
	printf("FUNCTION SIGNATURE is %s \n", prof->function_signature.c_str());
	printf("WORK is %llu \n", static_cast<unsigned long long>(prof->work));
	printf("COUNT is %d \n", prof->count);
	printf("================\n");
}


