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

// TODO: add offset to START TIME ALSO!!

#include "CallSiteProfile.h"

CallSiteProfile::CallSiteProfile() : call_site(0) {}

CallSiteProfile::~CallSiteProfile() {}

CallSiteProfile::CallSiteProfile (std::shared_ptr<CallSiteSpanProfile>
 																 span_profile,
 							     std::shared_ptr<CallSiteWorkProfile> 
 							      								 work_profile)
 							      								 : call_site(0) {
	call_site = work_profile->call_site;
	function_signature = work_profile->function_signature;
	lock_wait_time = span_profile->lock_wait_time;
	start = span_profile->start;
	work = work_profile->work + lock_wait_time;
	span = span_profile->span + lock_wait_time;
	stop = span_profile->start + span_profile->span;
	count = work_profile->count;
	if (work == 0)
		printf("ERROR in call site profile initialization: work cannot be zero \n");
	if (span == 0)
		printf("ERROR in call site profile initialization: span cannot be zero \n");
	parallelism = static_cast<double>(work) /
						static_cast<double>(span);
}

void CallSiteProfile::print() {
	printf(" ======================================================== \n");
	printf("CALL SITE %d \n", static_cast<int>(call_site));
	printf("FUNCTION SIGNATURE is %s \n", function_signature.c_str());
	printf("WORK is %llu \n", static_cast<unsigned long long>(work));
	printf("LOCK WAIT TIME is %llu \n", static_cast<unsigned long long>(lock_wait_time));
	printf("SPAN is %llu \n", static_cast<unsigned long long>(span));
	printf("LOCK WAIT TIME IS %f of SPAN \n", static_cast<double>(lock_wait_time) / 
									     	  static_cast<double>(span));
	printf("COUNT is %d \n", count);
	printf("START TIME is %d \n", start);
	printf("PARALLELISM is %f \n", static_cast<double>(work)/static_cast<double>(span));
	printf(" ======================================================== \n");
}


