/*
 * parasite.h
 *
 * Created on: June 16, 2016
 *      Author: knapp
 * 
 * Includes struct declarations for parasite profile, call site profile, call site end profile, and hashtables 
 * Includes method declarations for hasthable operations
 */


#ifndef CALL_SITE_HASHTABLE_H_
#define CALL_SITE_HASHTABLE_H_

#include <iostream>
#include <limits>
#include <map>
#include <stdbool.h>

#include "Types.h"
#include "CallSiteProfile.h"
#include "EndProfiles.h"

typedef std::map<CALLSITE, std::shared_ptr<call_site_profile_t>> call_site_hashtable_t;
typedef std::map<CALLSITE, std::shared_ptr<call_site_end_profile_t> > call_site_end_hashtable_t;

class CallSiteHashtable {

	public:

		CallSiteHashtable();
		CallSiteHashtable(std::shared_ptr<call_site_hashtable_t> hshtable);
		~CallSiteHashtable();

		// adds in hashtable into right hashtable
		void add_in_hashtable(const std::shared_ptr<call_site_hashtable_t> added_hashtable);

		// adds work, span, local_work, and local_span tto profile for call_site
		void add_data_to_hashtable(bool is_top_function,
		                         CALLSITE call_site,
		                         double work, double span,
		                         double local_work, double local_span);

		// adds local work and span to profile for call_site
		void add_local_data_to_hashtable(CALLSITE call_site,
		                                 double local_work, double local_span);

		std::shared_ptr<call_site_hashtable_t> hashtable;

  	private:

  		CallSiteHashtable(const CallSiteHashtable&);
		CallSiteHashtable& operator=(const CallSiteHashtable&);
};

#endif // CALL_SITE_HASHTABLE_H_


