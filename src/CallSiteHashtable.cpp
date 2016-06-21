#include "CallSiteHashtable.h"

// adds right hashtable into left hashtable
CallSiteHashtable::add_in_hashtable(const std::unique_ptr<call_site_hashtable_t> hashtable_to_add) {

	for (auto const &it : *hashtable_to_add) {

		CALLSITE key = it.first;
		std::unique_ptr<call_site_profile_t> profile_to_add = hashtable_to_add->at(key);

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable->count(key)) {

			std::unique_ptr<call_site_profile_t> profile = hashtable->at(key);
			combine_callsite_profile_entries(profile, profile_to_add);
		}

		// if not, insert added call site profile into the hashtable
		else  {
			std::pair<CALLSITE, std::unique_ptr<call_site_profile_t> > newPair;
			newPair.first = key;
			newPair.second = profile_to_add;
			hashtable->insert(newPair);
		}
	}

	return hashtable;
}

// add given call site profile data to the hashtable 
CallSiteHashtable::add_data_to_hashtable(bool is_top_function,
                         CALLSITE call_site,
                         double work, double span,
                         double local_work, double local_span) {

	if (hashtable->count(call_site)) {

		std::unique_ptr<call_site_profile_t> profile = hashtable->at(call_site);
		profile->local_count += 1;
  		profile->local_work += local_work;
  		profile->local_span += local_span;
  	
  		profile->count += 1;
  		profile->work += work;
  		profile->span += span;

  		if (is_top_function) {
			profile->top_count += 1;
			profile->top_work += work;
  			profile->top_span += span;
  		}
	}

	else {

		std::unique_ptr<call_site_profile_t> new_profile = create_callsite_profile(call_site, is_top_function, work, span, local_work, local_span);
		std::pair<CALLSITE, std::unique_ptr<call_site_profile_t>> newPair(call_site, new_profile);
		hashtable->insert(newPair);
	}
}

// add given call site profile data (only for local variables) to the hashtable 
CallSiteHashtable::add_local_data_to_hashtable(CALLSITE call_site,
                            		  double local_work, double local_span) {

	if (hashtable->count(call_site)) {

		std::unique_ptr<call_site_profile_t> profile = hashtable->at(call_site);

		profile->local_count += 1;
  		profile->local_work += local_work;
  		profile->local_span += local_span;
	}

	else {

		// THROW EXCEPTION

		std::cout << "ERROR: cannot add local values to hashtable entry that doesn't already exist" << std::endl;
	}
}


