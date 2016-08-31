/**
 *
 *    @file  CallSiteProfile.h
 *   @brief  Declarations for the CallSiteProfile class
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_PROFILE_H_
#define CALL_SITE_PROFILE_H_

#include <limits>
#include <memory>
#include <string>
#include <unordered_map>

#include "CallSiteSpanProfile.h"
#include "CallSiteWorkProfile.h"
#include "Types.h"

/**
*   @struct call_site_profile_t
*   @brief Work and span information describing a call site in the program.
*/

class CallSiteProfile {
    
 public:
    explicit CallSiteProfile(std::shared_ptr<CallSiteSpanProfile>
                                                                 span_profile,
                             std::shared_ptr<CallSiteWorkProfile> 
                                                                 work_profile);

    CallSiteProfile();
    ~CallSiteProfile();

    void print();
    

    /**
    *    @var call_site
    *    @brief The call site whose data is contained in the profile. 
    */
    CALLSITE call_site;


    /**
    *    @var count
    *    @brief Number of invocations of callsite. 
    */
    int count;

    /**
    *    @var function_signature 
    *    @brief The function signature of the call site whose data is contained
                in the profile. 
    */
    FUN_SG function_signature;


    /**
    *    @var parallelism
    *    @brief Parallelism of callsite.  
    */
    double parallelism;


    /**
    *    @var lock_wait_time
    *    @brief Lock wait time of callsite. 
    */
    TIME lock_wait_time;

    /**
    *    @var span
    *    @brief Span of callsite. 
    */
    TIME span;

    /**
    *    @var work
    *    @brief Work of callsite. 
    */
    TIME work;

    TIME start;
    TIME stop;

 private:
    CallSiteProfile(const CallSiteProfile&);
    CallSiteProfile& operator=(const CallSiteProfile&);
};



#endif  // CALL_SITE_PROFILE_H_
