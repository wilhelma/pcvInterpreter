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

CallSiteProfile::CallSiteProfile(std::shared_ptr<call_site_span_profile_t>
 																 span_profile,
 							     std::shared_ptr<call_site_work_profile_t> 
 							      								 work_profile) {

	std::shared_ptr<call_site_profile_t> prf(new call_site_profile_t());
	prof = prf;
	prof->call_site = work_profile->call_site;
	prof->function_signature = work_profile->function_signature;
	prof->lock_wait_time = span_profile->lock_wait_time;
	prof->start = span_profile->start;
	prof->stop = span_profile->stop;
	prof->work = static_cast<TIME>(work_profile->work);
	prof->span = static_cast<TIME>(span_profile->span + prof->lock_wait_time);
	prof->count = work_profile->count;
	if (prof->work == 0)
		printf("ERROR in call site profile initialization: work cannot be zero \n");
	if (prof->span == 0)
		printf("ERROR in call site profile initialization: span cannot be zero \n");
	prof->parallelism = static_cast<double>(prof->work) /
						static_cast<double>(prof->span);
}

void CallSiteProfile::print() {
	printf(" ================ \n");
	printf("CALL SITE %d \n", static_cast<int>(prof->call_site));
	printf("FUNCTION SIGNATURE is %s \n", prof->function_signature.c_str());
	printf("WORK is %llu \n", static_cast<unsigned long long>(prof->work));
	printf("LOCK WAIT TIME is %llu \n", static_cast<unsigned long long>(prof->lock_wait_time));
	printf("SPAN is %llu \n", static_cast<unsigned long long>(prof->span));
	printf("LOCK WAIT TIME IS %f of SPAN \n", static_cast<double>(prof->lock_wait_time) / 
									     static_cast<double>(prof->span));
	printf("COUNT is %d \n", prof->count);
	printf("PARALLELISM is %f \n", static_cast<double>(prof->work)/static_cast<double>(prof->span));
	printf("================\n");
}


