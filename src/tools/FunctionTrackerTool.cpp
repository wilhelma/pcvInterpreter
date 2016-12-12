/**
 *
 *    @file  FunctionTrackerTool.cpp
 *   @brief  Implementation file for the class `FunctionTrackerTool`
 *
 *    @date  04/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "FunctionTrackerTool.h"

#include "CallInfo.h"
#include "NewThreadInfo.h"
#include "ShadowThread.h"

#include <iostream>


/**
 * @brief Constructor for `FunctionTrackerTool`
 * 
 * Opens the output file and writes the "header" for a .dot graph.
 *
 * @todo Check for errors/exceptions
 */
FunctionTrackerTool::FunctionTrackerTool(std::string outFileName, std::string graphName):
	_outFileName(outFileName), _graphName(graphName)
{
	// open output file
	_outFile.open(_outFileName);
	_outFile
		<< "// Nodes with the same name are superposed so" << std::endl 
		<< "// you can safely keep single-entry lines" << std::endl 
		<< "graph " << _graphName << " {" << std::endl;
}

/**
 * @brief Desctructor for `FunctionTrackerTool`
 * 
 * Closes the output file.
 * @todo Check for errors/exceptions
 */
FunctionTrackerTool::~FunctionTrackerTool() {
	_outFile << "}" << std::endl;
	_outFile.close();
} 

void FunctionTrackerTool::NewThread(const NewThreadEvent* e ) {
  std::cerr << "New Thread... parent: " << e->threadId()
            << " child: " << e->info()->childThreadId();
            // << " runtime: " << info->runtime << std::endl;
}

void FunctionTrackerTool::Call(const CallEvent* e) {
	// This needs to be printed anyway
	_outFile << e->info()->fnSignature << "_" << e->info()->segment; // _lastSegmentNumber; //;
	if( _lastSegmentNumber < (int)e->info()->segment ) // connect to branch
		_outFile << " -- ";
	else // -------------------------------------------------------- terminate branch
		_outFile << ";" << std::endl;

	// update counter for last segment
	_lastSegmentNumber = (int)e->info()->segment;
	// update number of recorded calls
	std::cout << ++ calls << std::endl;
}
