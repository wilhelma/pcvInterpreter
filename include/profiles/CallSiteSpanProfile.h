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

struct call_site_span_profile_t {

    /**
    *    @var call_site
    *    @brief The call site whose data is contained in the profile. 
    */
    CALLSITE call_site;

    /**
    *    @var span
    *    @brief Span of callsite. 
    */
    TIME span;


    /**
    *    @var span
    *    @brief Lock span of callsite. 
    */
    TIME lock_span;

    call_site_span_profile_t() : call_site(0), span(0), lock_span(0) {}  
};

class CallSiteSpanProfile {
    
 public:
    explicit CallSiteSpanProfile(std::shared_ptr<call_site_span_profile_t> profile);
    ~CallSiteSpanProfile();

    void print();

    /**
    *    @fn add_in_callsite_profile_entries(const std::shared_ptr<call_site_span_profile_t> profile_to_add)
    *    @brief Add the entries in profile_to_add to prof
    */
    void add_in_callsite_span_profile_entries(const std::shared_ptr<call_site_span_profile_t> profile_to_add);

    /**
    *    @fn init_callsite_profile(...)
    *    @brief Initialize prof with the information provided in function parameters
    */
    void init_callsite_span_profile(CALLSITE call_site, 
                                    TIME span,
                                    TIME lock_span);
    
    /**
    *    @var prof
    *    @brief The call_site_span_profile_t that the profile's methods act on. 
    */
    std::shared_ptr<call_site_span_profile_t> prof;

 private:
    CallSiteSpanProfile(const CallSiteSpanProfile&);
    CallSiteSpanProfile& operator=(const CallSiteSpanProfile&);
};



#endif  // CALL_SITE_SPAN_PROFILE_H_
