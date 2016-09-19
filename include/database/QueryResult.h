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

#include <sqlite3.h>

/// @ingroup input
/// @brief Holds a prepared query statement and accesses its values.
class QueryResult {
public:
    /// @brief Constructor.
    /// @param query_statement The prepared query statement (i.e. the result of a query). 
    QueryResult(sqlite3_stmt* query_statement)
        : Query_(query_statement)
    {}

    /// @brief Accesses the value in the prepared statement.
    /// @param column The column to access in the prepared statement.
    template<typename T>
    const T get(int column) const;

    /// Makes a step, i.e. reads the next entry in the prepared statement.
    const int step()
    { return sqlite3_step(Query_); }

    /// @brief Destructor: tries to finalize the prepared statement.
    /// @throw SQLException
    /// @todo Find a way not to throw from the destructor!
    ~QueryResult() {
        if (finalize() != SQLITE_OK)
            throw SQLException("Cannot finalize prepared statament.", "QueryResult::~QueryResult");
    };

private:
    /// @brief Pointer to the query results.
    /// @todo Try to wrap Query_ in a smart pointer (e.g. unique_ptr)
    sqlite3_stmt* Query_;

    /// Finalizes the prepared statement.
    int finalize()
    { return sqlite3_finalize(Query_); }
};

template<typename T>
inline const T QueryResult::get(int column) const
{ return T(sqlite3_column_int(Query_, column)); }

/// @brief Accesses the query (`std::string` specialization).
/// @param column The column to access in the query.
template<>
inline const std::string QueryResult::get(int column) const
{ return std::string(reinterpret_cast<const char*>(sqlite3_column_text(Query_, column))); }

/// @brief Accesses the query (`TIME` specialization).
/// @param column The column to access in the query.
template<>
inline const uint64_t QueryResult::get(int column) const
{ return uint64_t(sqlite3_column_int64(Query_, column)); }

#endif
