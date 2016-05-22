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
	const FUN_SG parentFunctionSignature;
	// function signature
	const FUN_SG functionSignature;
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
typedef std::map<const FUN_SG, WorkSpan_* > WorkSpanMap;


// class to contain information used in Parasite algorithm
class Parasite {

	public:
					   
		const WorkSpanMap getWorkSpanMap() const;
		~Parasite();

		// void add a function and it's corresponding WorkSpan_ struct
		void addWorkSpan(const FUN_SG parentFunctionSignature, const FUN_SG functionSignature, double work, double prefix, double longest_child, double continuation);

		// change WorkSpan for function with signature functionSignature
		void setWorkSpan(const FUN_SG functionSignature, double work_diff, double prefix_diff, double longest_child_diff, double continuation_diff);

		// increment WorkSpan variables for function with signature functionSignature
		void addToWorkSpan(const FUN_SG functionSignature, double work_diff, double prefix_diff, double longest_child_diff, 
	 								double continuation_diff);

		double getWork(const FUN_SG functionSignature);
		double getPrefix(const FUN_SG functionSignature);
		double getLongestChild(const FUN_SG functionSignature);
		double getContinuation(const FUN_SG functionSignature);
		double getLockSpan(const FUN_SG functionSignature);
		double getLongestChildLockSpan(const FUN_SG functionSignature);
		double getLastLockStart(const FUN_SG functionSignature);

		const FUN_SG getSignature(int threadID);

		void setWork(const FUN_SG functionSignature, double work);
		void setPrefix(const FUN_SG functionSignature, double prefix);
		void setLongestChild(const FUN_SG functionSignature, double longest_child);
		void setContinuation(const FUN_SG functionSignature, double continuation);
		void setLockSpan(const FUN_SG functionSignature, double lock_span);
		void setLongestChildLockSpan(const FUN_SG functionSignature, double longest_child_lock_span);
		void setLastLockStart(const FUN_SG functionSignature, double last_lock_start);

		void addToWork(const FUN_SG functionSignature, double work_diff);
		void addToPrefix(const FUN_SG functionSignature, double prefix_diff);
		void addToLongestChild(const FUN_SG functionSignature, double longest_child_diff);
		void addToContinuation(const FUN_SG functionSignature, double continuation_diff);
		void addToLockSpan(const FUN_SG functionSignature, double lock_span_diff);
		void addToLongestChildLockSpan(const FUN_SG functionSignature, double longest_child_lock_span_diff);
		void addToLastLockStart(const FUN_SG functionSignature, double last_lock_start_diff);

	private:

		WorkSpanMap workSpanMap;
		// prevent generated functions
		Parasite(const Parasite&);
		Parasite& operator=(const Parasite&);
};

#endif /* Parasite_H_ */
