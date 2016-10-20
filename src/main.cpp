/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "DebugTool.h"
#include "Filter.h"
#include "FunctionTrackerTool.h"
#include "ParasiteTool.h"
#include "SAAPRunner.h"
//#include "LockSetChecker.h"
//#include "RaceDetectionTool.h"

// Loggin system
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

/// @brief The main routine.
/// @attention The file name of the database to interpret is ___mandatory!___
/// @param argv[0] The name of the executable.
/// @param argv[1] The name of the database to interpret.
int main(int argc, char* argv[]) {

    // check arguments
    if (argc < 2) {
        LOG(FATAL) << "No database name provided!";
        return 1;
    }

    std::cout << "STARTING PARCEIVE " << std::endl;

    // Create a SAAPRunner
    auto runner(std::make_unique<SAAPRunner>());

    // Create and register the tool
    const auto& debug_tool_it = runner->registerTool(
            std::make_unique<DebugTool>(),
            std::unique_ptr<Filter>(nullptr),
            Events::ALL);

//    const auto& parasite_tool_it = runner->registerTool(
//            std::make_unique<ParasiteTool>(),
//            std::unique_ptr<Filter>(nullptr),
//            Events::ALL);

    std::cout << " BEFORE INTERPRET " << std::endl;

    // Start interpretation of the database
    runner->interpret(static_cast<std::string>(argv[1]));


    std::cout << " AFTER INTERPRET " << std::endl;

    // Unregister the tool
    runner->removeTool(debug_tool_it);
//    runner->removeTool(parasite_tool_it);

    std::cout << " AFTER REMOVING TOOLS " << std::endl;

    return 0;
}
