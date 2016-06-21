// all the information that parasite provides after exceuting its algorithm
struct parasite_profile_t {

	// TODO: add commments explaning what each of these means
	double work;
	double span;
	double lock_span;
	double parallelism;

};

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

  	// Local lock span associated with call_site
  	double local_lock_span;

  	// Number of invocations of call_site, excluding recursive instances.
  	int count;

  	// Number of top-level invocations of call_site.
  	int top_count;

  	// Work associated with call_site, excluding recursive instances
  	double work;

  	// Span associated with call_site, excluding recursive instances
  	double span;

  	// Lock span associated with call_site, excluding recursive instances
  	double lock_span;

  	// Work associated with top-level invocations of call_site
  	double top_work;

  	// Span associated with top-level invocations of call_site
  	double top_span;

  	// Lock span associated with top-level invocations of call_site
  	double top_lock_span;

  	call_site_profile_t(){};

};

struct call_site_end_profile_t {

	// TODO: provide more detailed explanations of variable names

	// work excluding recurisve calls
	double work_work;
	double span_work;
	double parallelism_work;
	double count_work;

	// work data from top calls of call site
	double top_work_work;
	double top_lock_span_work;
	double top_span_work;
	double top_parallelism_work;
	double top_count_work;

	// local work call site data
	double local_work_work;
	double local_lock_span_work;
	double local_span_work;
	double local_parallelism_work;
	double local_count_work;


	// span data from top calls of call site
	double top_work_span;
	double top_lock_span_span;
	double top_span_span;
	double top_parallelism_span;
	double top_count_span;

	// local(?) span call site data
	double local_work_span;
	double local_lock_span_span;
	double local_span_span;
	double local_parallelism_span;
	double local_count_span;

	// span data excluding recursive calls
	double work_span;
	double span_span;
	double lock_span_span;
	double parallelism_span;
	double count_span;

	call_site_end_profile_t(){};

};