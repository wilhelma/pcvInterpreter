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

CallSiteSpanProfile::CallSiteSpanProfile() : call_site(0), span(0), 
											 lock_wait_time(0) {}

CallSiteSpanProfile::~CallSiteSpanProfile() {}

void CallSiteSpanProfile::add_in_callsite_span
			  (const std::shared_ptr<CallSiteSpanProfile> profile_to_add) {
	span += profile_to_add->span;
}

void CallSiteSpanProfile::init(CALLSITE cll_site) {
	call_site = cll_site;
}

void CallSiteSpanProfile::print() {
	std::cout << " ================ \n" << std::endl;
	std::cout << "CALL SITE " << static_cast<int>(call_site) << std::endl;
	print_time("SPAN", span);
	print_time("LOCK WAIT TIME", lock_wait_time);
	std::cout << "LOCK SPAN FRACTION of SPAN \n" << 
									static_cast<double>(lock_wait_time) / 
								    static_cast<double>(span) << std::endl;
	std::cout << "================\n" << std::endl;
}