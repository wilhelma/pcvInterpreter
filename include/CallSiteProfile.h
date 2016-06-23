#ifndef CALL_SITE_PROFILE_H_
#define CALL_SITE_PROFILE_H_

#include <map>
#include "Types.h"

struct call_site_profile_t {
    // call site ID 
    CALLSITE call_site;

    // true is function is recursive, false otherwise
    bool isRecursive;

    // Number of invocations of call_site, excluding recursive instances.
    int count;

    // Total number of invocations of call_site
    int local_count;

    // Number of top-level invocations of call_site.
    int top_count;

    // Work associated with call_site, excluding recursive instances
    double work;

    // Span associated with call_site, excluding recursive instances
    double span;

    // Lock span associated with call_site, excluding recursive instances
    double lock_span;

    // Local lock span associated with call_site
    double local_lock_span;

    // Local span associated with call_site
    double local_span;

    // Local work associated with call_site
    double local_work;

    // Work associated with top-level invocations of call_site
    double top_work;

    // Span associated with top-level invocations of call_site
    double top_span;

    // Lock span associated with top-level invocations of call_site
    double top_lock_span;
};

class CallSiteProfile {
 public:
    explicit CallSiteProfile(std::shared_ptr<call_site_profile_t> profile);
    ~CallSiteProfile();

    void add_in_callsite_profile_entries(const 
                          std::shared_ptr<call_site_profile_t> profile_to_add);

    void init_callsite_profile(CALLSITE call_site, bool is_top_function, 
                                                    double work, double span,
                                         double local_work, double local_span);

    std::shared_ptr<call_site_profile_t> prof;

 private:
    CallSiteProfile(const CallSiteProfile&);
    CallSiteProfile& operator=(const CallSiteProfile&);
};



#endif  // CALL_SITE_PROFILE_H_
