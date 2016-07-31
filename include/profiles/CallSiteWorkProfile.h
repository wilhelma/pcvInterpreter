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

#ifndef call_site_work_profile_H_
#define call_site_work_profile_H_

#include <unordered_map>
#include <memory>
#include "Types.h"

/**
*   @struct call_site_work_profile_t
*   @brief Work and span information describing a call site in the program.
*/

struct call_site_work_profile_t {

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

    call_site_work_profile_t() : call_site(0), work(0), count(0) {}  
};

class CallSiteWorkProfile {
    
 public:
    explicit CallSiteWorkProfile(std::shared_ptr<call_site_work_profile_t> profile);
    ~CallSiteWorkProfile();

    void print();

    /**
    *    @fn add_in_callsite_profile_entries(const std::shared_ptr<call_site_work_profile_t> profile_to_add)
    *    @brief Add the entries in profile_to_add to prof
    */
    void add_in_callsite_profile_entries(const std::shared_ptr<call_site_work_profile_t> profile_to_add);

    /**
    *    @fn init_callsite_profile(...)
    *    @brief Initialize prof with the information provided in function parameters
    */
    void init_callsite_profile(CALLSITE call_site, FUN_SG function_signature, TIME work);
    
    /**
    *    @var prof
    *    @brief The call_site_work_profile_t that the profile's methods act on. 
    */
    std::shared_ptr<call_site_work_profile_t> prof;

 private:
    CallSiteWorkProfile(const CallSiteWorkProfile&);
    CallSiteWorkProfile& operator=(const CallSiteWorkProfile&);
};



#endif  // call_site_work_profile_H_
