/*
 * ParasiteTool.h
 *
 *  Created on: June 17, 2016
 *      Author: knapp
 */

#ifndef PARASITE_TOOL_H_
#define PARASITE_TOOL_H_

#include <iostream>
#include <array>
#include <set>
#include <map>
#include <vector>
#include <memory>

#include "Interpreter.h"
#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "Tool.h"
#include "DataModel.h"
#include "DBDataModel.h"
#include "ParasiteStacks.h"
 
class ParasiteTool : public Tool {

public:
	
	// contains both function stack and thread stack 
	main_stack_t *main_stack;

	// keeps track of the index for each function
	std::map<unsigned int, int> lock_hashtable;

	// contains profile information at end of tool 
	parasite_profile_t *parasite_profile;
	call_site_end_hashtable_t* end_call_site_profile_hashtable;

	TIME last_strand_start_time;
	int total_locks_running;

	ParasiteTool();
	~ParasiteTool();

	void create(const Event* e);
	void join(const Event* e);
	void acquire(const Event* e);
	void release(const Event* e);
	void access(const Event* e);
	void call(const Event* e);
	void returnOfCalled(const Event* e);
	void threadEnd(const Event* e);
	void printProfile();
	void getEndProfile();

private:

	// prevent generated functions --------------------------------------------
	ParasiteTool(const ParasiteTool&);
	ParasiteTool& operator=(const ParasiteTool&);
};

#endif /* PARASITE_TOOL_H_ */
