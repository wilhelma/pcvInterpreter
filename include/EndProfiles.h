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

// all the information that parasite provides after exceuting its algorithm
struct parasite_profile_t {
	// TODO(knapp): add commments explaning what each of these means
	double work;
	double span;
	double lock_span;
	double parallelism;

	parasite_profile_t() {}
};

struct call_site_end_profile_t {
	// TODO(knapp): provide more detailed explanations of variable names

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

	call_site_end_profile_t() {}
};

#endif  // END_PROFILES_H_
