/*
 * Parasite.h
 *
 *  Created on: May 8, 2016
 *      Author: knapp
 */

#ifndef Parasite_H_
#define Parasite_H_

#include <vector>	

// maps function signatures to their FunctionInfo_ structure
typedef std::map<const char*, WorkSpan_* > WorkSpanMap;

// stores the work-span variables of a function
typedef struct WorkSpan_ {

	// parent function signature
	const char* parentFunctionSignature;
	// function signature
	const char* functionSignature;
	// work 
	double work;
	// prefix
	double prefix;
	// longest-child
	double longest_child;
	// continuation
	double continuation;
	// span on locks
	double lock_span;
	// span of longest child
	double longest_child_lock_span;
	// runtime at beginning of most recent lock encountered by function
	double last_lock_start;

} WorkSpan_;

// class to contain information used in Parasite algorithm
class Parasite {

	public:
					   
		const WorkSpanMap getWorkSpanMap() const;
		Parasite();
		~Parasite();

		// add a function and it's corresponding WorkSpan_ struct
		addWorkSpan(const char* functionSignature, double work, double prefix, double longest-child, double continuation);

		// change WorkSpan for function with signature functionSignature
		setWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest-child_diff, double continuation_diff);

		// increment WorkSpan variables for function with signature functionSignature
		addToWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest-child_diff, 
	 								double continuation_diff);

		double getWork(const char* functionSignature);
		double getPrefix(const char* functionSignature);
		double getLongestChild(const char* functionSignature);
		double getContinuation(const char* functionSignature);
		double getLockSpan(const char* functionSignature);
		double getLongestChildLockSpan(const char* functionSignature);
		double getLastLockStart(const char* functionSignature);

		setWork(const char* functionSignature, double work);
		setPrefix(const char* functionSignature, double prefix);
		setLongestChild(const char* functionSignature, double longest_child);
		setContinuation(const char* functionSignature, double continuation);
		setLockSpan(const char* functionSignature, double lock_span);
		setLongestChildLockSpan(const char* functionSignature, double longest_child_lock_span);
		setLastLockStart(const char* functionSignature, double last_lock_start);

		addToWork(const char* functionSignature, double work_diff);
		addToPrefix(const char* functionSignature, double prefix_diff);
		addToLongestChild(const char* functionSignature, double longest_child_diff);
		addToContinuation(const char* functionSignature, double continuation_diff);
		addToLockSpan(const char* functionSignature, double lock_span_diff);
		addToLongestChildLockSpan(const char* functionSignature, double longest_child_lock_span_diff);
		addToLastLockStart(const char* functionSignature, double last_lock_start_diff);

	private:

		WorkSpanMap workSpanMap;
		// prevent generated functions
		Parasite(const Parasite&);
		Parasite& operator=(const Parasite&);
};

#endif /* Parasite_H_ */
