/**
 *
 *    @file  CallSiteProfile.cpp
 *   @brief  Implementation file for the class `CallSiteProfile`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp) << 
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "CallSiteProfile.h"

CallSiteProfile::CallSiteProfile() : call_site(0), lock_wait_time(0), span(0), work(0), start(0), stop(0) {}

CallSiteProfile::~CallSiteProfile() {}

CallSiteProfile::CallSiteProfile (std::shared_ptr<CallSiteSpanProfile>
 																 span_profile,
 							     std::shared_ptr<CallSiteWorkProfile> 
 							      								 work_profile,
 							     TIME start_time)
 							      								 : 
 							      								 call_site(0),  
 							      								 lock_wait_time(0),  
 							      								 span(0),
 							      								 work(0),
									 							 start(start_time),
									 							 stop(0) {
	call_site = work_profile->call_site;
	function_signature = work_profile->function_signature;
	lock_wait_time = span_profile->lock_wait_time;
	work = work_profile->work;
	span = span_profile->span;
	stop = start + span_profile->span;
	count = work_profile->count;
	if (work == 0)
		std::cout << "ERROR in call site profile initialization: work cannot be zero " << std::endl;
	if (span == 0) {
		std::cout << "ERROR in call site profile initialization: span cannot be zero " << std::endl;
		span = 1;
	};
	parallelism = static_cast<double>(work) /
				  static_cast<double>(span);
}

void CallSiteProfile::print() {
	std::cout << " ======================================================== " << std::endl;
	std::cout << "CALL SITE " <<  static_cast<int>(call_site) << std::endl;
	std::cout << "FUNCTION SIGNATURE is " <<  function_signature.c_str() << std::endl;
	print_time("WORK", work);
	print_time("LOCK_WAIT_TIME", lock_wait_time);
	print_time("SPAN", span);
	std::cout << "LOCK WAIT TIME PERCENTAGE OF SPAN " <<  static_cast<double>(lock_wait_time) / 
									     	  		      static_cast<double>(span) << std::endl;
	std::cout << "COUNT " <<  count << std::endl;
	print_time("START TIME", start);
	std::cout << "PARALLELISM " <<  static_cast<double>(work)/static_cast<double>(span) << std::endl;
	std::cout << " ======================================================== " << std::endl;
}


