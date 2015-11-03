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
#include "RaceDetectionTool.h"
#include "LockSetChecker.h"
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
	LockSetChecker *raceTool = new LockSetChecker("races.json");
	runner->registerTool(raceTool, NULL, ALL);

	// Start interpretation
	runner->interpret();

	delete interpreter;
	delete service;
	delete runner;
	delete raceTool;

	return 0;
}
