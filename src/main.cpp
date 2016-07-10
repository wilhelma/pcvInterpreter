/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include <boost/log/trivial.hpp>
#include "SAAPRunner.h"
#include "EventService.h"
#include "DBInterpreter.h"
//#include "RaceDetectionTool.h"
//#include "LockSetChecker.h"
#include "DebugTool.h"
#include "FunctionTrackerTool.h"
#include "ParasiteTool.h"
#include "LockMgr.h"
#include "ThreadMgr.h"

int main(int argc, char* argv[]) {

	// check arguments
	if (argc < 2) {
		BOOST_LOG_TRIVIAL(fatal) << "No database name provided!";
		return 1;
	}

	// create interpreter, event service, and saap runner
	EventService *service = new EventService();
	LockMgr *lockMgr = new LockMgr();
	ThreadMgr *threadMgr = new ThreadMgr();
	DBInterpreter *interpreter = new DBInterpreter(argv[1],
												   "SAAP.log",
												   service,
												   lockMgr,
												   threadMgr);
	
	SAAPRunner *runner = new SAAPRunner(interpreter);

	// create and register tools
	//RaceDetectionTool *raceTool = new RaceDetectionTool("races.json");
	//LockSetChecker *raceTool = new LockSetChecker("races.json");
	FunctionTrackerTool *functionTool = new FunctionTrackerTool();

	ParasiteTool *parasiteTool = new ParasiteTool();

	DebugTool *debugTool = new DebugTool();

	// register functionTool, no filters, all events except ACCESS
	runner->registerTool(parasiteTool, NULL,
						 Events::CALL | Events::NEWTHREAD | 
						 Events::THREADEND | Events::JOIN | 
						 Events::ACQUIRE | Events::RELEASE | 
						 Events::RETURN );

	runner->registerTool(debugTool, NULL,
						 Events::CALL | Events::NEWTHREAD | 
						 Events::THREADEND | Events::JOIN | 
						 Events::ACQUIRE | Events::RELEASE | 
						 Events::RETURN | Events::ACCESS );

	// register functionTool, no filters, only CALL and NEWTHREAD events
  	// runner->registerTool(functionTool, NULL,
   //                     	Events::CALL | Events::NEWTHREAD );

	// Start interpretation
	runner->interpret();

	// unregister
	runner->removeTool(functionTool);

	parasiteTool->printProfile();
	runner->removeTool(parasiteTool);

	runner->removeTool(debugTool);

	delete interpreter;
	delete service;
	delete runner;
	// delete functionTool;
	delete parasiteTool;
	delete debugTool;

	return 0;
}
