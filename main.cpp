/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "SAAPRunner.h"
#include "EventService.h"
#include "DBInterpreter.h"
#include "RaceDetectionTool.h"

int main(int argc, char** argv) {

	// create interpreter, event service, and saap runner
	DBInterpreter *interpreter =
			new DBInterpreter("/home/wilhelma/workspace/test/SAAP.db");
	EventService *service = new EventService();
	SAAPRunner *runner = new SAAPRunner(interpreter, service);

	// create and register tools
	RaceDetectionTool *raceTool = new RaceDetectionTool();
	runner->registerTool(raceTool);

	// Start interpretation
	runner->interpret();

	delete interpreter;
	delete service;
	delete runner;
	delete raceTool;

	return 0;
}
