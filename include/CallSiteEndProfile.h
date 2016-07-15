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
	uint64_t work;

	/**
	*    @var span
	*    @brief Span of non-recursive executions of callsite.
	*/
	uint64_t span;

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

	/**
	*    @var top_work
	*    @brief Work of top callsite invocations.
	*/
	uint64_t top_work;

	/**
	*    @var top_lock_span
	*    @brief Lock span of top callsite invocations.
	*/
	uint64_t top_lock_span;

	/**
	*    @var top_span
	*    @brief Span of top callsite invocations.
	*/
	uint64_t top_span;

	/**
	*    @var top_parallelism
	*    @brief Parallelism of top callsite invocations.
	*/
	double top_parallelism;

	/**
	*    @var top_count
	*    @brief Number of top invocations of callsite.
	*/
	int top_count;


	/**
	*    @var local_work
	*    @brief Local work of callsite (excluding work of functions callsite invokes).
	*/
	uint64_t local_work;

	/**
	*    @var local_lock_span
	*    @brief Local lock span of callsite (excluding lock span of functions callsite invokes).
	*/
	uint64_t local_lock_span;

	/**
	*    @var local_span
	*    @brief Local span of callsite (excluding span of functions callsite invokes).
	*/
	uint64_t local_span;

	/**
	*    @var local_parallelism
	*    @brief Local parallelism of callsite (calculated from work and span excluding parallelism of functions callsite invokes).
	*/
	double local_parallelism;

	/**
	*    @var local_count
	*    @brief Local count of callsite invocations (excluding callsite invocations from functions callsite invokes).
	*/
	int local_count;

	/**
	*    @var work_on_span
	*    @brief Work of non-recursive executions of callsite on the critical path of the program. 
	*/
	uint64_t work_on_span;

	/**
	*    @var span_on_span
	*    @brief Span of non-recursive executions of callsite on the critical path of the program. 
	*/
	uint64_t span_on_span;

	/**
	*    @var lock_span_on_span
	*    @brief Lock span of non-recursive executions of callsite on the critical path of the program. 
	*/
	uint64_t lock_span_on_span;

	/**
	*    @var parallelism_on_span
	*    @brief Parallelism of non-recursive executions of callsite on the critical path of the program. 
	*/
	double parallelism_on_span;

	/**
	*    @var count_on_span
	*    @brief Invocation count of non-recursive executions of callsite on the critical path of the program. 
	*/
	int count_on_span;

	/**
	*    @var top_work_on_span
	*    @brief Top work of callsite on the critical path of the program. 
	*/
	uint64_t top_work_on_span;

	/**
	*    @var top_lock_span_on_span
	*    @brief Top lock span of callsite on the critical path of the program. 
	*/
	uint64_t top_lock_span_on_span;

	/**
	*    @var top_span_on_span
	*    @brief Top span of callsite on the critical path of the program. 
	*/
	uint64_t top_span_on_span;

	/**
	*    @var top_span_on_span
	*    @brief Top parallelism of callsite on the critical path of the program. 
	*/
	double top_parallelism_on_span;

	/**
	*    @var top_count_on_span
	*    @brief Top invocation counts of callsite on the critical path of the program. 
	*/
	int top_count_on_span;

	/**
	*    @var local_work_on_span
	*    @brief Local work of callsite (excluding work of functions callsite invokes)
				on the critical path of the program.
	*/
	uint64_t local_work_on_span;

	/**
	*    @var local_lock_span_on_span
	*    @brief Local lock span of callsite (excluding lock span of functions callsite invokes)
				on the critical path of the program.
	*/
	uint64_t local_lock_span_on_span;

	/**
	*    @var local_span_on_span
	*    @brief Local span of callsite (excluding span of functions callsite invokes)
				on the critical path of the program.
	*/
	uint64_t local_span_on_span;

	/**
	*    @var local_parallelism_on_span
	*    @brief Local parallelism of callsite (excluding parallelism of functions callsite invokes)
				on the critical path of the program.
	*/
	double local_parallelism_on_span;

	/**
	*    @var local_count_on_span
	*    @brief Local invocation count of callsite (excluding invocation count of functions callsite invokes)
				on the critical path of the program.
	*/
	int local_count_on_span;

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
