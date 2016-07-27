/**
 *
 *    @file  QueryResult.h
 *   @brief  
 *
 *    @date  07/21/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  QUERY_RESULT_H_
#define  QUERY_RESULT_H_

#include "SQLException.h"

#include <iostream>
#include <memory>
#include <sqlite3.h>

/// @class QueryResult
/// @brief Manages the result of a query to a database.
class QueryResult {
public:
	/// Constructor.
	QueryResult(sqlite3_stmt* query_statement)
		: Query_(query_statement)
	{}

	/// @brief Accesses the query.
	/// @param column The column to access in the query.
	template<typename T>
	const T get(int column) const;

	/// Makes a step, i.e. reads the next entry.
	const int step()
	{ return sqlite3_step(Query_); }

	/// Destructor: throws if `QueryResult::finalize` fails.
	~QueryResult() {
		if (finalize() != SQLITE_OK) {
			std::cerr << "[~QueryResult] " << "Cannot finalize prepared statament" << std::endl;
			throw SQLException("Cannot finalize prepared statament.", "QueryResult::~QueryResult");
		}
	};

private:
	/// @brief Pointer to the query results.
	/// @todo Try to wrap Query_ in a smart pointer (e.g. unique_ptr)
	sqlite3_stmt* Query_;

	/// Finalizes the prepared statement.
	int finalize()
	{ return sqlite3_finalize(Query_); }

	/// Constructor (private).
	QueryResult();
};

template<typename T>
inline const T QueryResult::get(int column) const
{ return T(sqlite3_column_int(Query_, column)); }

/// @brief Accesses the query (`std::string` specialization).
/// @param column The column to access in the query.
template<>
inline const std::string QueryResult::get(int column) const
{ return std::string(reinterpret_cast<const char*>(sqlite3_column_text(Query_, column))); }

#endif
