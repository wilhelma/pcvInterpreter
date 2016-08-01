
#ifndef PARASITE_PROFILE_H_
#define PARASITE_PROFILE_H_

#include "Types.h"

/**
*    @struct parasite_profile_t
*    @brief Work and span information describing entire program being profiled. 

*/
struct parasite_profile_t {

	/**
	*    @var lock_span
	*    @brief The total lock span (the total runtime, scaled, on the criical
				path, spent while at least one lock is engaged) of all 
				instructions executed in the program.
	*/
	TIME lock_span;

	/**
	*    @var parallelism
	*    @brief The parallelism of the program. This is the work divided by span,
				which is an upper bound of the program's speedup on any number
				of processors.
	*/
	double parallelism;


	/**
	*    @var span
	*    @brief The span (the total runtime, scaled, on the criical path) 
				of all instructions executed in the program.
	*/
	TIME span;

	/**
	*    @var work
	*    @brief The work (the total runtime, scaled) of all instructions
				executed in the program.
	*/
	TIME work;

	parasite_profile_t(): lock_span(0), parallelism(0), span(0), work(0) {}
};

#endif /* PARASITE_PROFILE_H_ */

