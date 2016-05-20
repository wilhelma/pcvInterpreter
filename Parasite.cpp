/*
 * Parasite.cpp
 *
 *  Created on: Dec 12, 2015
 *      Author: knapp
 */

#include <vector>
#include "Parasite.h"

Parasite::Parasite() {
	
	workSpanMap = new WorkSpanMap();
}	

Parasite::~Parasite() {

	workSpanMap.clear();
}

Parasite::Parasite::addWorkSpan(const char* functionSignature, double work, double prefix, double longest-child, double continuation)  {

	WorkSpan_ newWorkSpan = {functionSignature, work, prefix, longest-child, continuation, 0.0, 0.0};
	workSpanMap.insert(std::pair<char*, WorkSpan> (functionSignature, &newWorkSpan));
}

Parasite::setWorkSpan(const char* functionSignature, double work, double prefix, double longest-child, double continuation)  {

	workSpanMap.erase(functionSignature);
	WorkSpan_ newWorkSpan = {functionSignature, work, prefix, longest-child, continuation, 0.0, 0.0};
	workSpanMap.insert(std::pair<char*, WorkSpan> (functionSignature, &newWorkSpan));
}

Parasite::addToWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest-child_diff, 
	 								double continuation_diff)  {

	workSpanMap.erase(functionSignature);
	WorkSpan_ newWorkSpan = {functionSignature, work + work_diff, prefix + prefix_diff, longest-child + longest-child_diff, 
		                     continuation + continuation_diff};
	workSpanMap.insert(std::pair<char*, WorkSpan> (functionSignature, &newWorkSpan));
}

Parasite::setWork(const char* functionSignature, double wrk){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->work = wrk;
}

Parasite::setPrefix(const char* functionSignature, double prfix){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->prefix = prfix;
}

Parasite::setLongestChild(const char* functionSignature, double lngest_child){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child = lngest_child;
}

Parasite::setContinuation(const char* functionSignature, double cntinuation){


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->continuation = cntinuation;
}

Parasite::setLockSpan(const char* functionSignature, double lck_span) {


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->lock_span = lck_span;
}

Parasite::setLongestChildLockSpan(const char* functionSignature, double lngest_child_lock_span) {


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child_lock_span = lngest_child_lock_spans;
}

Parasite::setLastLockStart(const char* functionSignature, double lst_lock_start) {


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->last_lock_start = lst_lock_start;
}


Parasite::addToWork(const char* functionSignature, double work_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->work += work_diff;
}

Parasite::addToPrefix(const char* functionSignature, double prefix_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->prefix += prefix_diff;
}

Parasite::addToLongestChild(const char* functionSignature, double longest_child_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child += longest_child_diff;
}

Parasite::addToContinuation(const char* functionSignature, double continuation_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->continuation += continuation_diff;
}

Parasite::addToLockSpan(const char* functionSignature, double lock_span_diff) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->lock_span += lock_span_diff;
}

Parasite::addToLongestChildLockSpan(const char* functionSignature, double longest_child_lock_span_diff) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child_lock_span += longest_child_lock_span_diff;
}


Parasite::addToLastLockStart(const char* functionSignature, double last_lock_start_diff) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->last_lock_start += last_lock_start_diff;
}

double Parasite::getWork(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->work;
}

double Parasite::getPrefix(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->prefix;
}

double Parasite::getLongestChild(const char* functionSignature){

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->longest_child;
}

double Parasite::getContinuation(const char* functionSignature){

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->continuation;
}

double Parasite::getLockSpan(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->lock_span;
}

double Parasite::getLongestChildLockSpan(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->longest_child_lock_span;
}






