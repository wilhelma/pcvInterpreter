/*
 * DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#include <iostream>
#include "DBInterpreter.h"

int instructionCB(void *data, int argc, char **argv, char **colName) {

	DBInterpreter* interpreter = (DBInterpreter*) data;

    for(int i=0; i<argc; i++){
	      std::cerr << colName[i] << " = " << argv[i] << std::endl;
    }
    std::cerr << std::endl;

	return 0;
}

bool DBInterpreter::loadDB(const char* path, sqlite3 *db) {

	char *errMsg = 0;

	if (sqlite3_open(path, &db)) {
		sqlite3_close(db);
		return false;
	} else
		return true;
}

bool DBInterpreter::closeDB(sqlite3 *db) {

	sqlite3_close(db);
	return true;
}

int DBInterpreter::process() {

	sqlite3 *db;

	if (!loadDB(_dbPath, db))
		return 1;

	fillStructures(db);

	closeDB(db);

	return 0;
}

int DBInterpreter::fillStructures(sqlite3 *db) {

	int rc;

	if ((rc = fillInstruction(db)) != 0) return rc;
	if ((rc = fillCall(db)) != 0) return rc;

	return 0;
}

int DBInterpreter::fillInstruction(sqlite3 *db) {

   char *errMsg = 0;
   int rc;
   char *sql;

   sql = "SELECT * from INSTRUCTION_TABLE";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, instructionCB, (void*)this, &errMsg);
   if( rc != SQLITE_OK ){
	  std::cerr << "SQL error: " << errMsg << std::endl;
	  sqlite3_free(errMsg);
	  return 1;
   }

	return 0;
}

int DBInterpreter::fillCall(sqlite3 *db) {

	return 0;
}

//class DBInterpreter : public Interpreter {
//public:
//	DBInterpreter(const char* DBPath) : _dbPath(DBPath) { }
//	virtual void process();
//
//private:
//	const char* _dbPath;
//
//	// prevent generated functions
//	DBInterpreter(const DBInterpreter&);
//	DBInterpreter& operator=(const DBInterpreter&);
//};


