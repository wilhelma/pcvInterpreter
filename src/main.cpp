/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */


#include "DBInterpreter.h"
#include "DebugTool.h"
#include "EventService.h"
#include "ParasiteTool.h"
//#include "RaceDetectionTool.h"
//#include "LockSetChecker.h"
#include "FunctionTrackerTool.h"
#include "LockMgr.h"
#include "SAAPRunner.h"
#include "ThreadMgr.h"
#include <boost/log/trivial.hpp>


int main(int argc, char* argv[]) {

	// check arguments
	if (argc < 2) {
		BOOST_LOG_TRIVIAL(fatal) << "No database name provided!";
		return 1;
	}

	// create interpreter, event service, and saap runner
    std::shared_ptr<EventService> service(new EventService());
    std::shared_ptr<LockMgr> lockMgr(new LockMgr());
    std::shared_ptr<ThreadMgr> threadMgr(new ThreadMgr());
    std::shared_ptr<DBInterpreter> interpreter(new DBInterpreter(argv[1],
			"SAAP.log",
			service.get(),
			lockMgr.get(),
			threadMgr.get()));

    std::shared_ptr<SAAPRunner> runner(new SAAPRunner(interpreter.get()));

	// create and register tools
	// RaceDetectionTool *raceTool = new RaceDetectionTool("races.json");
	// LockSetChecker *raceTool = new LockSetChecker("races.json");
	// FunctionTrackerTool *functionTool = new FunctionTrackerTool();

	// register functionTool, no filters, all events except ACCESS
    std::shared_ptr<ParasiteTool> parasiteTool(new ParasiteTool());
	runner->registerTool(parasiteTool.get(), NULL,Events::ALL);

    std::shared_ptr<DebugTool> debugTool(new DebugTool());
	runner->registerTool(debugTool.get(), NULL, Events::ALL);

	// Start interpretation
	runner->interpret();

	// unregister
	runner->removeTool(parasiteTool.get());
	runner->removeTool(debugTool.get());
	return 0;
}
