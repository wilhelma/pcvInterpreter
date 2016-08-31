/**
 *
 *    @file  CallSiteSpanProfile.h
 *   @brief  Declarations for the CallSiteSpanProfile class
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_SPAN_PROFILE_H_
#define CALL_SITE_SPAN_PROFILE_H_

#include <unordered_map>
#include <memory>
#include "Types.h"

/**
*   @struct call_site_span_profile_t
*   @brief Span information describing a call site in the program.
*/

class CallSiteSpanProfile {
    
 public:

    CallSiteSpanProfile();
    ~CallSiteSpanProfile();

    void print();

    /**
    *    @fn add_in_callsite_profile_entries(const std::shared_ptr<call_site_span_profile_t> profile_to_add)
    *    @brief Add the entries in profile_to_add to prof
    */
    void add_in_callsite_span (const std::shared_ptr<CallSiteSpanProfile> 
                                      profile_to_add);

    /**
    *    @fn init_callsite_profile(...)
    *    @brief Initialize prof with the information provided in function parameters
    */
    void init(CALLSITE call_site, TIME start_time);

    /**
    *    @var call_site
    *    @brief The call site whose data is contained in the profile. 
    */
    CALLSITE call_site;

    TIME start;

    /**
    *    @var span
    *    @brief Span of callsite. 
    */
    TIME span;

    /**
    *    @var lock_wait_time
    *    @brief Lock wait time of callsite. 
    */
    TIME lock_wait_time;

 private:
    CallSiteSpanProfile(const CallSiteSpanProfile&);
    CallSiteSpanProfile& operator=(const CallSiteSpanProfile&);
};



#endif  // CALL_SITE_SPAN_PROFILE_H_
