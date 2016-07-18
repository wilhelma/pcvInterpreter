/*
 * Interpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "fwd/EventService.h"
#include "fwd/LockMgr.h"
#include "fwd/ThreadMgr.h"

/******************************************************************************
 * Constants and Types
 *****************************************************************************/
typedef enum {
	IN_OK = 0,				// Okay.
	IN_NO_ENTRY = 1,		// No entry found.
	IN_ENTRY_EXISTS = 2,	// Entry already exists.
	IN_ABORT = 3			// Operation aborted.
} OkCode;

/******************************************************************************
 * Interpreter (abstract)
 *****************************************************************************/
class Interpreter {
	public:
		Interpreter(LockMgr* lockMgr, ThreadMgr* threadMgr, const char* logFile); 
		virtual int process() = 0;
		void initLogger() const;

		virtual ~Interpreter() {}
		virtual EventService* getEventService() = 0;

	protected:
		LockMgr* lockMgr_;
		ThreadMgr* threadMgr_;

	private:
		const char* logFile_;
};


#endif /* INTERPRETER_H_ */
