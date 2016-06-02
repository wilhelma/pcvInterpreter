/*
 * ParasiteTool.h
 *
 *  Created on: Dec 12, 2015
 *      Author: knapp
 */

#ifndef ParasiteTOOL_H_
#define ParasiteTOOL_H_

#define BURDENING 0

#include <vector>
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
#include "Types.h"

#include "ParasiteToolHelper.h"
#include "ParasiteToolUtilities.h"
#include "ParasiteHashtable.h"
 
class ParasiteTool : public Tool {

public:
	
	parasite_stack_t *main_stack;

	TIME last_lock_start;
	TIME last_strand_start;

	int min_capacity;

	ParasiteTool();
	~ParasiteTool();

	// Events already in all Tools
	void create(const Event* e);
	void join(const Event* e);
	void acquire(const Event* e);
	void release(const Event* e);
	void access(const Event* e);
	void call(const Event* e);

	// Events added for this tool specifically
	void returnOfCalled(const Event* e);
	void threadEnd(const Event* e);
	void print();

	unsigned long long getCurrentCallSite();
	ShadowThread* getCurrentThread();
	FUN_SG getCurrentFunctionSignature();
	
private:

	// keeps track of current call site
	CALLSITE currentCallSiteID;

	// keeps track of current thread
	ShadowThread* currentThread;

	// keeps track of current function signature 
	FUN_SG currentFunctionSignature;

	// prevent generated functions --------------------------------------------
	ParasiteTool(const ParasiteTool&);
	ParasiteTool& operator=(const ParasiteTool&);
};



#endif /* ParasiteTOOL_H_ */
