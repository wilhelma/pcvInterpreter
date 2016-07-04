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

#include <unordered_map>
#include <memory>
#include "Types.h"

/**
*   @struct call_site_profile_t
*   @brief Work and span information describing a call site in the program.

    A measurement for a call site s without prefixes "top" or "local" aggregates the work and span of
    every execution of s that is not a recursive execution of s.

    A measurement for a call site s with prefix "top" aggregates the work and span of every execution 
    of s from a nonrecursive invocation of its caller.

    A measurement for a call site s with prefix "local_" is the work in Trace(s) minus the work in the
    traces of the functions that s invokes. 
*/

struct call_site_profile_t {

    /**
    *    @var call_site
    *    @brief The call site whose data is contained in the profile. 
    */
    CALLSITE call_site;

    /**
    *    @var work
    *    @brief Work of non-recursive executions of callsite. 
    */
    double work;

    /**
    *    @var span
    *    @brief Span of non-recursive executions of callsite.
    */
    double span;

    /**
    *    @var count
    *    @brief Number of non-recurisve invocations of callsite. 
    */
    double count;

    /**
    *    @var top_work
    *    @brief Work of top callsite invocations.
    */
    double top_work;

    /**
    *    @var top_lock_span
    *    @brief Lock span of top callsite invocations.
    */
    double top_lock_span;

    /**
    *    @var top_span
    *    @brief Span of top callsite invocations.
    */
    double top_span;

    /**
    *    @var top_count
    *    @brief Number of top invocations of callsite.
    */
    double top_count;


    /**
    *    @var local_work
    *    @brief Local work of callsite (excluding work of functions callsite invokes).
    */
    double local_work;

    /**
    *    @var local_lock_span
    *    @brief Local lock span of callsite (excluding lock span of functions callsite invokes).
    */
    double local_lock_span;

    /**
    *    @var local_span
    *    @brief Local span of callsite (excluding span of functions callsite invokes).
    */
    double local_span;

    /**
    *    @var local_count
    *    @brief Local count of callsite invocations (excluding callsite invocations from functions callsite invokes).
    */
    double local_count;

};

class CallSiteProfile {
    
 public:
    explicit CallSiteProfile(std::shared_ptr<call_site_profile_t> profile);
    ~CallSiteProfile();

    /**
    *    @fn add_in_callsite_profile_entries(const std::shared_ptr<call_site_profile_t> profile_to_add)
    *    @brief Add the entries in profile_to_add to prof
    */
    void add_in_callsite_profile_entries(const std::shared_ptr<call_site_profile_t> profile_to_add);

    /**
    *    @fn init_callsite_profile(...)
    *    @brief Initialize prof with the information provided in function parameters
    */
    void init_callsite_profile(CALLSITE call_site, bool is_top_function, 
                                                    double work, double span,
                                                    double local_work, double local_span);
    /**
    *    @var prof
    *    @brief The call_site_profile_t that the profile's methods act on. 
    */
    std::shared_ptr<call_site_profile_t> prof;

 private:
    CallSiteProfile(const CallSiteProfile&);
    CallSiteProfile& operator=(const CallSiteProfile&);
};



#endif  // CALL_SITE_PROFILE_H_
