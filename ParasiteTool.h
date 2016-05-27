/*
 * ParasiteTool.h
 *
 *  Created on: Dec 12, 2015
 *      Author: knapp
 */

#ifndef ParasiteTOOL_H_
#define ParasiteTOOL_H_

#include "Tool.h"
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
#include "DataModel.h"
#include "DBDataModel.h"
#include "ParasiteHashtable.h"

#include "Parasite.h"
#include "ParasiteStacks.h"


#define THREADS 100

typedef std::map<TRD_ID, const FUN_SG> ThreadFunctionMap;

class ParasiteTool : public Tool {

public:

	parasite_stack_t *main_stack;
	iaddr_table_t *call_site_table;
	iaddr_table_t *function_table;
	int MIN_CAPACITY;

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
	void print();
	const FUN_SG getSignature(TRD_ID id);
	
private:

	// keeps track of current thread
	ShadowThread* currentThread;

	// keeps track of current function signature for each thread
	ThreadFunctionMap threadFunctionMap;

	// prevent generated functions --------------------------------------------
	ParasiteTool(const ParasiteTool&);
	ParasiteTool& operator=(const ParasiteTool&);
};



#endif /* ParasiteTOOL_H_ */
