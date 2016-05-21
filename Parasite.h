/*
 * Parasite.h
 *
 *  Created on: May 8, 2016
 *      Author: knapp
 */

#ifndef Parasite_H_
#define Parasite_H_

#include <vector>	

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
	// longest_child
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

// maps function signatures to their FunctionInfo_ structure
typedef std::map<const char*, WorkSpan_* > WorkSpanMap;


// class to contain information used in Parasite algorithm
class Parasite {

	public:
					   
		const WorkSpanMap getWorkSpanMap() const;
		~Parasite();

		// void add a function and it's corresponding WorkSpan_ struct
		void addWorkSpan(const char* parentFunctionSignature, const char* functionSignature, double work, double prefix, double longest_child, double continuation);

		// change WorkSpan for function with signature functionSignature
		void setWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest_child_diff, double continuation_diff);

		// increment WorkSpan variables for function with signature functionSignature
		void addToWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest_child_diff, 
	 								double continuation_diff);

		double getWork(const char* functionSignature);
		double getPrefix(const char* functionSignature);
		double getLongestChild(const char* functionSignature);
		double getContinuation(const char* functionSignature);
		double getLockSpan(const char* functionSignature);
		double getLongestChildLockSpan(const char* functionSignature);
		double getLastLockStart(const char* functionSignature);

		void setWork(const char* functionSignature, double work);
		void setPrefix(const char* functionSignature, double prefix);
		void setLongestChild(const char* functionSignature, double longest_child);
		void setContinuation(const char* functionSignature, double continuation);
		void setLockSpan(const char* functionSignature, double lock_span);
		void setLongestChildLockSpan(const char* functionSignature, double longest_child_lock_span);
		void setLastLockStart(const char* functionSignature, double last_lock_start);

		void addToWork(const char* functionSignature, double work_diff);
		void addToPrefix(const char* functionSignature, double prefix_diff);
		void addToLongestChild(const char* functionSignature, double longest_child_diff);
		void addToContinuation(const char* functionSignature, double continuation_diff);
		void addToLockSpan(const char* functionSignature, double lock_span_diff);
		void addToLongestChildLockSpan(const char* functionSignature, double longest_child_lock_span_diff);
		void addToLastLockStart(const char* functionSignature, double last_lock_start_diff);

	private:

		WorkSpanMap workSpanMap;
		// prevent generated functions
		Parasite(const Parasite&);
		Parasite& operator=(const Parasite&);
};

#endif /* Parasite_H_ */
