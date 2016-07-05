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

	ParasiteTool *ParasiteTool = new ParasiteTool();

	// register functionTool, no filters, only CALL events
  	runner->registerTool(functionTool, NULL,
                       Events::CALL | Events::NEWTHREAD );

	// Start interpretation
	runner->interpret();

	// unregister
	runner->removeTool(functionTool);

	delete interpreter;
	delete service;
	delete runner;
	delete functionTool;

	return 0;
}
