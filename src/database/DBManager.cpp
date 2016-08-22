/**
 *
 *    @file  DBManager.cpp
 *   @brief  
 *
 *    @date  07/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "DBManager.h"

#include "QueryResult.h"
#include "SQLException.h"

#include <iostream>
#include <sqlite3.h>
#include <string>

void DBManager::open(const std::string& DBPath) {
//	// temporary database pointer
//	sqlite3 *temp_db_pointer = nullptr;

	// open the database and store its return code
	auto rc = sqlite3_open_v2(DBPath.c_str(),
			&Database_,
			SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX,
			nullptr);

//	// store the pointer in a unique_ptr class
//	Database_.reset(temp_db_pointer);

	// if database opening failed
	if (rc != SQLITE_OK) {
		std::cerr << sqlite3_errmsg(Database_) << std::endl;
		throw SQLException(sqlite3_errmsg(Database_), "DBManager::DBManager");
	}
}

DBManager::~DBManager() {
	auto rc = sqlite3_close(Database_);
	if (rc != SQLITE_OK) {
		std::cerr << "[~DBManager] " << sqlite3_errmsg(Database_) << std::endl;
		throw SQLException(sqlite3_errmsg(Database_), "DBManager::~DBManager");
	}
}

std::shared_ptr<QueryResult> DBManager::query(const std::string& sql_query) const {
	sqlite3_stmt *sql_statement = nullptr;

	// Execute SQL statement
	auto rc = sqlite3_prepare_v2(Database_,
			sql_query.c_str(),
			sql_query.length(),
			&sql_statement,
			nullptr);

	if (rc != SQLITE_OK) {
		std::cerr << sqlite3_errmsg(Database_) << std::endl;
		throw SQLException(sqlite3_errmsg(Database_), "DBManager::query");
	}

	// this will be moved, not copied
	return std::shared_ptr<QueryResult>(new QueryResult(sql_statement));
}
