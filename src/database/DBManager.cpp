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
#include "SQLStatementIterator.h"

#include "easylogging++.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <sqlite3.h>
#include <string>

void DBManager::open(const std::string& DBPath) {
//	// temporary database pointer
//	sqlite3 *temp_db_pointer = nullptr;

	// open the database and store its return code
	auto rc = sqlite3_open_v2(DBPath.c_str(),
			&Database_,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX,
			nullptr);

//	// store the pointer in a unique_ptr class
//	Database_.reset(temp_db_pointer);

	// if database opening failed
	if (rc != SQLITE_OK)
		throw SQLException(sqlite3_errmsg(Database_), "DBManager::DBManager");
}

// Helper function to close the database
void close_database(sqlite3* db) {
	if (sqlite3_close(db) != SQLITE_OK)
		throw SQLException(sqlite3_errmsg(db), "DBManager::~DBManager");
}

DBManager::~DBManager() noexcept {
    try { close_database(Database_); }
    catch (SQLException& e) {
        std::cerr << e.what() << std::endl;
        std::abort();
    }
}

std::unique_ptr<QueryResult> DBManager::query(const std::string& sql_query) const {
	sqlite3_stmt *sql_statement = nullptr;

	// Execute SQL statement
	auto rc = sqlite3_prepare_v2(Database_,
			sql_query.c_str(),
			sql_query.length(),
			&sql_statement,
			nullptr);

	if (rc != SQLITE_OK)
		throw SQLException(sqlite3_errmsg(Database_), "DBManager::query");

	return std::make_unique<QueryResult>(sql_statement);
}

const size_t entries(const std::string& table_name, const DBManager& connection) {
    auto it = SQLStatementIterator<size_t>(connection.query("select count(*) from " + table_name));
    const auto entries = *it;
    assert(++it == SQLStatementIterator<size_t>::end());
    return entries;
}

const DBManager open_database_connection(const std::string& DBPath) {
    DBManager db;
    try {
        db.open(DBPath);
    } catch (const SQLException& e) {
        LOG(FATAL) << e.what();
        std::abort();
    }
    return db;
}
