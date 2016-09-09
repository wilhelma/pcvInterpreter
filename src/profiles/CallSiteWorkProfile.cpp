/**
 *
 *    @file  CallSiteWorkProfile.cpp
 *   @brief  Implementation file for the class `CallSiteWorkProfile`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp) << 
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "CallSiteWorkProfile.h"


CallSiteWorkProfile::CallSiteWorkProfile() : call_site(0), work(0), count(1) {}

CallSiteWorkProfile::~CallSiteWorkProfile() {}

void CallSiteWorkProfile::add_in_callsite_profile_entries(const std::shared_ptr<CallSiteWorkProfile> profile_to_add) {
	work += profile_to_add->work;
	count += (profile_to_add->count - 1);
}

void CallSiteWorkProfile::init_callsite_profile(CALLSITE call_ste, FUN_SG function_sgnature) {
	call_site = call_ste;
	function_signature = function_sgnature;
	work = static_cast<TIME>(0);
	count = 0;
}

void CallSiteWorkProfile::print() {
	std::cout << " ================" << std::endl;
	std::cout << "CALL SITE is " << static_cast<int>(call_site) << std::endl;
	std::cout << "FUNCTION SIGNATURE is " <<  function_signature.c_str() << std::endl;
	print_time("WORK", work);
	std::cout << "COUNT is " <<  count << std::endl;
	std::cout << "================" << std::endl;
}


