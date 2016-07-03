/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

// #include "RaceDetectionTool.h"
// #include "LockSetChecker.h"

#include <boost/log/trivial.hpp>
#include "SAAPRunner.h"
#include "EventService.h"
#include "DBInterpreter.h"

#include "ParasiteTool.h"
#include "FunctionTrackerTool.h"
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

	// ParasiteTool *parasiteTool = new ParasiteTool();

	FunctionTrackerTool *functionTool = new FunctionTrackerTool();

	// register functionTool, no filters, only CALL and NEWTHREAD events
    runner->registerTool(functionTool, NULL,
                       Events::CALL | Events::NEWTHREAD );

    // register parasiteTool, TODO: check if filters are applicable here 
    // runner->registerTool(parasiteTool, NULL,
    //                    Events::CALL | Events::NEWTHREAD | 
    //                    Events::ACQUIRE | Events::RELEASE |
    //                    Events::JOIN );


    printf("after registering function Tool");
	
	// Start interpretation
	runner->interpret();

	printf("after interpret()");

	// unregister
	runner->removeTool(functionTool);
	// runner->removeTool(parasiteTool);

	delete interpreter;
	delete service;
	delete runner;
	delete functionTool;
	// delete parasiteTool;

	return 0;
}
