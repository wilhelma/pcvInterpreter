/**
 *
 *    @file  CallSiteWorkProfile.h
 *   @brief  Declarations for the CallSiteWorkProfile class
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_WORK_PROFILE_H_
#define CALL_SITE_WORK_PROFILE_H_

#include <unordered_map>
#include <memory>
#include "Types.h"


class CallSiteWorkProfile {
    
 public:

    CallSiteWorkProfile();
    ~CallSiteWorkProfile();
    void print();

    /**
    *    @fn add_in_callsite_profile_entries(const std::shared_ptr<CALL_SITE_WORK_PROFILE_H_t> profile_to_add)
    *    @brief Add the entries in profile_to_add to prof
    */
    void add_in_callsite_profile_entries(const std::shared_ptr<CallSiteWorkProfile> profile_to_add);

    /**
    *    @fn init_callsite_profile(...)
    *    @brief Initialize prof with the information provided in function parameters
    */
    void init_callsite_profile(CALLSITE call_site, FUN_SG function_signature);
        /**
    *    @var call_site
    *    @brief The call site whose data is contained in the profile. 
    */
    CALLSITE call_site;


    /**
    *    @var function_signature 
    *    @brief The function singature of the call site whose data is contained
                in the profile. 
    */
    FUN_SG function_signature;

    /**
    *    @var work
    *    @brief Work of callsite. 
    */
    TIME work;

    /**
    *    @var count
    *    @brief Number of invocations of callsite. 
    */
    int count;

 private:
    CallSiteWorkProfile(const CallSiteWorkProfile&);
    CallSiteWorkProfile& operator=(const CallSiteWorkProfile&);
};



#endif  // CALL_SITE_WORK_PROFILE_H_
