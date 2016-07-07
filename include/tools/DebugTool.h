/**
 *
 *    @file  DebugTool.h
 *   @brief  Declarations for the class `DebugTool`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef DEBUG_TOOL_H_
#define DEBUG_TOOL_H_

#include <array>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "AccessEvent.h"
#include "CallEvent.h"
#include "Event.h"
#include "JoinEvent.h"
#include "NewThreadEvent.h"
#include "ReturnEvent.h"
#include "ThreadEndEvent.h"

#include "Interpreter.h"
#include "ParasiteStack.h"
#include "ShadowLock.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "Tool.h"


 /**
 *
 *    @class  DebugTool.h
 *    @brief  Prints debugging information as Events occur. 
 */

class DebugTool : public Tool {
 public:
	DebugTool();
	~DebugTool();

	void create(const Event* e);
	void join(const Event* e);
	void acquire(const Event* e);
	void release(const Event* e);
	void access(const Event* e);
	void call(const Event* e);
	void returnOfCalled(const Event* e);
	void threadEnd(const Event* e);

 private:
	// prevent generated functions --------------------------------------------
	DebugTool(const DebugTool&);
	DebugTool& operator=(const DebugTool&);
};

#endif  // DEBUG_TOOL_H_ 