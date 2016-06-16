#include <map>
#include "stdbool.h"

// all the information that parasite provides after exceuting its algorithm
struct parasite_profile_t {

	// TODO: add commments explaning what each of these means
	double work_on_work;
	double span_on_work;
	double parallelism_on_work;
	double count_on_work;

	double top_work_on_work;
	double top_span_on_work;
	double top_parallelism_on_work;
	double top_count_on_work;

	double local_work_on_work;
	double local_span_on_work;
	double local_parallelism_on_work;
	double local_count_on_work;

	double work_on_span;
	double span_on_span;
	double parallelism_on_span;
	double count_on_span;

	double top_work_on_span;
	double top_span_on_span;
	double top_parallelism_on_span;
	double top_count_on_span;

	double local_work_on_span;
	double local_span_on_span;
	double local_parallelism_on_span;
	double local_count_on_span;

}

struct call_site_profile_t {

	// call site ID 
	CALLSITE call_site;

	// true is function is recursive, false otherwise
	bool isRecursive;

	// Total number of invocations of call_site
	int local_count;

	// Local work associated with call_site
  	double local_work;

  	// Local span associated with call_site
  	double local_span;

  	// Number of invocations of call_site, excluding recursive instances.
  	int count;

  	// Number of top-level invocations of call_site.
  	int top_count;

  	// Work associated with call_site, excluding recursive instances
  	double work;

  	// Span associated with call_site, excluding recursive instances
  	double span;

  	// Work associated with top-level invocations of call_site
  	double top_work;

  	// Span associated with top-level invocations of call_site
  	double top_span;

}

// hashtable mapping call sites to their data
typedef std::map<CALLSITE, call_site_profile_t> call_site_hashtable_t;









