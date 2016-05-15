s/*
 * CilkProf.h
 *
 *  Created on: May 8, 2016
 *      Author: knapp
 */

#ifndef CILKPROF_H_
#define CILKPROF_H_

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

} WorkSpan_;

// class to contain information used in CilkProf algorithm
class CilkProf {

	public:
					   
		const WorkSpanMap getWorkSpanMap() const;
		CilkProf();
		~CilkProf();

		// add a function and it's corresponding WorkSpan_ struct
		addWorkSpan(const char* functionSignature, double work, double prefix, double longest-child, double continuation);

		// change WorkSpan for function with signature functionSignature
		setWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest-child_diff, double continuation_diff);

		// increment WorkSpan variables for function with signature functionSignature
		incrementWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest-child_diff, 
	 								double continuation_diff);

		double getWork(const char* functionSignature);
		double getPrefix(const char* functionSignature);
		double getLongestChild(const char* functionSignature);
		double getContinuation(const char* functionSignature);

		setWork(const char* functionSignature, double work);
		setPrefix(const char* functionSignature, double prefix);
		setLongestChild(const char* functionSignature, double longest_child);
		setContinuation(const char* functionSignature, double continuation);

		addToWork(const char* functionSignature, double work_diff);
		addToPrefix(const char* functionSignature, double prefix_diff);
		addToLongestChild(const char* functionSignature, double longest_child_diff);
		addToContinuation(const char* functionSignature, double continuation_diff);

	private:

		WorkSpanMap workSpanMap;
		// prevent generated functions
		CilkProf(const CilkProf&);
		CilkProf& operator=(const CilkProf&);
};

#endif /* CILKPROF_H_ */
