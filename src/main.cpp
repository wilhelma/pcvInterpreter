/*
 * main.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include "DebugTool.h"
#include "EventType.h"
#include "Filter.h"
#include "SAAPRunner.h"
//#include "FunctionTrackerTool.h"
//#include "LockSetChecker.h"
//#include "ParasiteTool.h"
//#include "RaceDetectionTool.h"

// Loggin system
#include "easylogging++.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

INITIALIZE_EASYLOGGINGPP

const auto  main_start = std::chrono::system_clock::now();
std::string elapsed_time = "";

const char* time_from_main(void) {
    using namespace std::chrono;
    const auto elapsed = duration_cast<milliseconds>(system_clock::now() - main_start).count();
    std::ostringstream output;
    output << std::setw(6) << std::right << elapsed << " ms";
    elapsed_time = output.str();
    return elapsed_time.c_str();
}

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

    el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%runtime", time_from_main));
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%runtime : %level >> %msg"); 

    std::cout << "STARTING PARCEIVE " << std::endl;

    // Create a SAAPRunner
    auto runner(std::make_unique<SAAPRunner>());

    // Create and register the tool
    const auto& debug_tool_it = runner->registerTool(
            std::make_unique<DebugTool>(),
            std::unique_ptr<Filter>(nullptr),
            EventType::ALL);

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
