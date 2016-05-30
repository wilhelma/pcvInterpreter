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

#include "Tool.h"
#include "Event.h"

#include <iostream>

#include "FunctionTrackerTool.h"
#include "ShadowThread.h"

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

void FunctionTrackerTool::create( const Event* e ) {
  const NewThreadEvent* event = static_cast<const NewThreadEvent*>(e);
  const NewThreadInfo* info = event->getNewThreadInfo();

  std::cerr << "New Thread... parent: " << info->parentThread->threadId
            << " child: " << info->childThread->threadId
            << " runtime: " << info->runtime << std::endl;
}

void FunctionTrackerTool::join( const Event* e ) {
}

void FunctionTrackerTool::acquire( const Event* e ) {
}

void FunctionTrackerTool::release( const Event* e ) {
}

void FunctionTrackerTool::access( const Event* e ) {
}

void FunctionTrackerTool::call( const Event* e ) {
    // cast event into its specific type
	const CallEvent *event = static_cast<const CallEvent*>(e);

	// This needs to be printed anyway
	_outFile << event->getCallInfo()->fnSignature << "_" << event->getCallInfo()->segment; // _lastSegmentNumber; //;
	if( _lastSegmentNumber < (int)event->getCallInfo()->segment ) // connect to branch
		_outFile << " -- ";
	else // -------------------------------------------------------- terminate branch
		_outFile << ";" << std::endl;

	// update counter for last segment
	_lastSegmentNumber = (int)event->getCallInfo()->segment;
	// update number of recorded calls
	std::cout << ++ calls << std::endl;

}
