/**
 *
 *    @file  EndProfiles.h
 *   @brief  Declarations for the structs parasite_profile_t and call_site_end_profile_t
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef END_PROFILES_H_
#define END_PROFILES_H_

#include "CallSiteProfile.h"
#include "Types.h"

/**
*   @call CallSiteEndProfile
*   @brief Work and span information describing a call site in the program.

	Any on_work variable accumulates work and span data for every invocation of
	a particular call site in the computation. 
	A record in the on-span profile accumulates work and span data only for 
	the invocations of a particular call site that appear on the critical path
	of the computation.

	A measurement for a call site s without prefixes "top" or "local" aggregates the work and span of
	every execution of s that is not a recursive execution of s.

	A measurement for a call site s with prefix "top" aggregates the work and span of every execution 
	of s from a nonrecursive invocation of its caller.

	A measurement for a call site s with prefix "local_" is the work in Trace(s) minus the work in the
	traces of the functions that s invokes. 

*/
class CallSiteEndProfile {

 public:
	/**
	*    @var call_site
	*    @brief The call site whose data is contained in the profile. 
	*/
	CALLSITE call_site;

	/**
	*    @var work
	*    @brief Work of non-recursive executions of callsite. 
	*/
	TIME work;

	/**
	*    @var span
	*    @brief Span of non-recursive executions of callsite.
	*/
	TIME span;

	/**
	*    @var parallelism
	*    @brief Parallelism (work/span) of non-recursive executions of callsite.
	*/
	double parallelism;

	/**
	*    @var count
	*    @brief Number of non-recurisve invocations of callsite. 
	*/
	int count;

	CallSiteEndProfile();
	explicit CallSiteEndProfile(std::shared_ptr<call_site_profile_t> collected_profile);
	~CallSiteEndProfile();

	void getEndCallSiteWorkProfile(std::shared_ptr<call_site_profile_t> collected_profile);
	void getEndCallSiteSpanProfile(std::shared_ptr<call_site_profile_t> collected_profile);

 private:

	CallSiteEndProfile(const CallSiteEndProfile&);
	CallSiteEndProfile& operator=(const CallSiteEndProfile&);
};

#endif  // END_PROFILES_H_
