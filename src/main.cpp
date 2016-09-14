/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */


#include "DBInterpreter.h"
#include "DebugTool.h"
//#include "EventService.h"
#include "ParasiteTool.h"
//#include "RaceDetectionTool.h"
//#include "LockSetChecker.h"
#include "FunctionTrackerTool.h"
//#include "LockMgr.h"
#include "SAAPRunner.h"
//#include "ThreadMgr.h"
#include <boost/log/trivial.hpp>
#include "Logger.h"

/// @brief The main routine.
/// @attention The file name of the database to interpret is ___mandatory!___
/// @param argv[0] The name of the executable.
/// @param argv[1] The name of the database to interpret.
int main(int argc, char* argv[]) {

    Logger<LogSeverity::debug> logger;

	// check arguments
	if (argc < 2) {
        logger.trace()   << "trace"   << std::endl;
        logger.warning() << "warning" << std::endl;
//        logger<LogSeverity::trace>()       << "trace()" << std::endl;

		BOOST_LOG_TRIVIAL(fatal) << "No database name provided!";
		return 1;
	}

	// Create a SAAPRunner
	auto runner(std::make_unique<SAAPRunner>(make_DBInterpreter("SAAP.log")));

    // Create and register the tool
	const auto& tool_it = runner->registerTool(
            std::make_unique<DebugTool>(),
            std::unique_ptr<Filter>(nullptr),
            Events::ALL);

	// Start interpretation of the database
	runner->interpret(static_cast<std::string>(argv[1]));

	// Unregister the tool
	runner->removeTool(tool_it);

	return 0;
}
