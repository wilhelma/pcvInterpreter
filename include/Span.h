/**
 *
 *    @file  Span.h
 *   @brief  Declarations for the class `Span`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef SPAN
#define SPAN

#include <stdbool.h>
#include <iostream>
#include <limits>
#include <functional>
#include <unordered_map>
#include <string>

#include "CallSiteHashtable.h"
#include "CallSiteProfile.h"
#include "Types.h"

/**
*    @typedef call_site_span_hashtable_t
*    @brief std::unordered_map used as a hashtable for matching CALLSITE keys
			to call_site_span_profile_t values. 
*/
typedef std::unordered_map<CALLSITE, std::shared_ptr<call_site_span_profile_t>, CallSiteHash, CallSiteEqual> call_site_span_hashtable_t;

class Span {

 public:
	Span();
	explicit Span(Span const& hshtable);
	
	Span* operator=(const Span& rhs);
	~Span();

	void print();

	/**
	*    @fn contains()
	*    @brief Returns true if the hashtable contains a profile for call_site.
	*/
	bool contains(CALLSITE call_site);


	/**
	*    @fn clear()
	*    @brief Clears the unordered_map / hashtable. 
	*/
	void clear();

	void set(Span* other_span);

	/**
	*    @fn add_in_hashtable(...)
	*    @brief Adds in added_hashtable into the class's hashtable.
	*/
	void add(Span* hashtable_object);

	/**
	*    @fn add_data(...)
	*    @brief Adds work and span to the profile for
			    call_site contained in hashtable.
	*/
	void add_to_call_site(CALLSITE call_site, TIME span);

	void set_lock_wait_time(CALLSITE call_site, TIME lock_wait_time);

	TIME operator()();

	TIME total;

	/**
	*    @var hashtable
	*    @brief A std::shared_ptr to a call_site_span_hashtable_t that maps
				CALLSITE keys to call_site_span_profile_t values. 
	*/
	std::shared_ptr<call_site_span_hashtable_t> hashtable;
	
};

#endif  // SPAN


