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

/// @brief Helper function to create a `DBInterpreter`.
/// @param logFileName The name of the log file.
/// @return A `std::unique_ptr` to a `DBInterpreter`.
inline decltype(auto) make_DBInterpreter(std::string&& logFileName) {
	return std::make_unique<DBInterpreter>(
			std::move(logFileName), std::make_shared<EventService>(),
			std::make_unique<LockMgr>(), std::make_unique<ThreadMgr>());
}

/// @brief The main routine.
/// @attention The file name of the database to interpret is ___mandatory!___
/// @param argv[0] The name of the executable.
/// @param argv[1] The name of the database to interpret.
int main(int argc, char* argv[]) {

	// check arguments
	if (argc < 2) {
		BOOST_LOG_TRIVIAL(fatal) << "No database name provided!";
		return 1;
	}

	// Create a SAAPRunner
	auto runner(std::make_unique<SAAPRunner>(make_DBInterpreter("SAAP.log")));

  //std::shared_ptr<ParasiteTool> parasiteTool(new ParasiteTool());
  //runner->registerTool(parasiteTool.get(), NULL, Events::ALL);

    auto debugTool(std::make_shared<DebugTool>());
	/// @todo Do in such a way that you don't need the tool address
	/// again to remove it. In this way the runner can have a unique_ptr
	/// to the tool!
	runner->registerTool(debugTool.get(), NULL, Events::ALL);

	// Start interpretation of the database
	runner->interpret(static_cast<std::string>(argv[1]));

	// unregister
  //unner->removeTool(parasiteTool.get());
	runner->removeTool(debugTool.get());
	return 0;
}
