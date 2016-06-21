/*
 * parasite.h
 *
 * Created on: June 16, 2016
 *      Author: knapp
 * 
 * Includes struct declarations for parasite profile, call site profile, call site end profile, and hashtables 
 * Includes method declarations for hasthable operations
 */


#ifndef PARASITE_H_
#define PARASITE_H_

#include <iostream>
#include <limits>
#include <map>
#include <stdbool.h>

#include "Types.h"

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


// hashtable mapping call sites to their data
typedef std::map<CALLSITE, call_site_profile_t*> call_site_hashtable_t;

// hashtable mapping call sites to their data in a format that is used for more statistics and is printed
typedef std::map<CALLSITE, call_site_end_profile_t*> call_site_end_hashtable_t;

// adds left hashtable into right hashtable
call_site_hashtable_t* add_call_site_hashtables(const call_site_hashtable_t *left, 
												call_site_hashtable_t *right);

// adds work, span, local_work, and local_span tto profile for call_site
void add_to_call_site_hashtable(bool is_top_function,
                         CALLSITE call_site,
                         double work, double span,
                         double local_work, double local_span, 
                         call_site_hashtable_t *table);

// adds local work and span to profile for call_site
void add_local_to_call_site_hashtable(CALLSITE call_site,
                            		  double local_work, double local_span,
                            		  call_site_hashtable_t *table);

#endif // PARASITE_H_ 








