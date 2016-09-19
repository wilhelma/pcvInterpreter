/**
 *
 *    @file  QueryResult.cpp
 *   @brief  
 *
 *    @date  09/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "QueryResult.h"

#include "SQLException.h"
#include <sqlite3.h>

#include <iostream>

// Helper function to finalize the prepared statement.
void finalize_query(sqlite3_stmt* query) {
    if (sqlite3_finalize(query) != SQLITE_OK)
        throw SQLException("Cannot finalize prepared statament.", "QueryResult::~QueryResult");
}

QueryResult::~QueryResult() noexcept {
    try { finalize_query(Query_); }
    catch (SQLException& e) {
        std::cerr << e.what() << std::endl;
        std::abort();
    }
}
