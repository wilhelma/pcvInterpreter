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
#include "Parasite.h"
#define THREADS 100

typedef std::map<unsigned int, const char*> ThreadFunctionMap;

class ParasiteTool : public Tool {
public:

	ParasiteTool();
	void create(const Event* e);
	void join(const Event* e);
	void acquire(const Event* e);
	void release(const Event* e);
	void access(const Event* e);
	void call(const Event* e);
	void returnOfCalled(const Event* e);
	void ~ParasiteTool();
	
private:

	// Provides information to the Parasite structure
	Parasite* parasite;

	// keeps track of current thread
	ShadowThread* currenThread;

	// keeps track of current function signature for each thread
	ThreadFunctionMap threadFunctionMap;

	// prevent generated functions --------------------------------------------
	ParasiteTool(const ParasiteTool&);
	ParasiteTool& operator=(const ParasiteTool&);
};



#endif /* ParasiteTOOL_H_ */
