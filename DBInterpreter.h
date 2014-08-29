/*
 * DBInterpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef DBINTERPRETER_H_
#define DBINTERPRETER_H_

#include <sqlite3.h>
#include "Interpreter.h"

/******************************************************************************
 * Database Interpreter
 *****************************************************************************/
class DBInterpreter : public Interpreter {
public:
	DBInterpreter(const char* DBPath) : _dbPath(DBPath) { }
	int process() override;

private:
	const char* _dbPath;

	// types
	struct instruction {
		int id;
		int segment_id;
		char instruction_type[10];
		int line_number;
	};

	struct call {
		char id[50];
		int process_id;
		int thread_id;
		int function_id;
		int instruction_id;
		char start_time[12];
		char end_time[12];
	};


	// private methods
	bool loadDB(const char* path, sqlite3 *db);
	bool closeDB(sqlite3 *db);
	int fillStructures(sqlite3 *db);
	int fillInstruction(sqlite3 *db);
	int fillCall(sqlite3 *db);

	friend int instructionCB(void *data, int argc, char **argv, char **colName);

	// prevent generated functions
	DBInterpreter(const DBInterpreter&);
	DBInterpreter& operator=(const DBInterpreter&);
};


#endif /* DBINTERPRETER_H_ */
