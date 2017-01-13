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
#include "Types.h"

#include <sqlite3.h>
#include <string>

/// @ingroup input
/// @brief Holds a prepared query statement and accesses its values.
class QueryResult {
public:
    /// @brief Constructor.
    /// @param query_statement The prepared query statement (i.e. the result of a query). 
    explicit QueryResult(sqlite3_stmt* query_statement)
        : Query_(query_statement)
    {}

    /// @brief Accesses the value in the prepared statement.
    /// @param column The column to access in the prepared statement.
    template<typename T>
    const T get(int column) const;

    /// @brief Accesses the value in the prepared statement for a single result.
    /// @param column The column to access in the prepared statement.
    template<typename T>
    const T getSingle(int column);

    /// Makes a step, i.e. reads the next entry in the prepared statement.
    const int step()
    { return sqlite3_step(Query_); }

    /// @brief Destructor: tries to finalize the prepared statement.
    /// @attention If an exception is thrown when finalizing the statement, the 
    /// execution will be `std::abort()`ed.
    ~QueryResult() noexcept;

private:
    /// @brief Pointer to the query results.
    /// @todo Try to wrap Query_ in a smart pointer (e.g. unique_ptr)
    sqlite3_stmt* Query_;
};

template<typename T>
inline const T QueryResult::getSingle(int column) {
    switch(step()) {
        case SQLITE_ROW:
            break;
        case SQLITE_DONE:
            throw SQLException("Iterating DB failed. No Entry.", "QueryResult::getSingle");
        default:
            throw SQLException("Iterating DB failed", "QueryResult::getSingle");
    }

    return static_cast<T>(sqlite3_column_int(Query_, column));
}

template<typename T>
inline const T QueryResult::get(int column) const
{ return static_cast<T>(sqlite3_column_int(Query_, column)); }

/// @brief Accesses the query (`std::string` specialization).
/// @param column The column to access in the query.
template<>
inline const std::string QueryResult::get(int column) const
{ return static_cast<std::string>(reinterpret_cast<const char*>(sqlite3_column_text(Query_, column))); }

template<>
inline const FastString<unsigned char> QueryResult::get(int column) const
{ return static_cast<FastString<unsigned char>>(sqlite3_column_text(Query_, column)); }

/// @brief Accesses the query (`TIME` specialization).
/// @param column The column to access in the query.
template<>
inline const TIME QueryResult::get(int column) const
{ return static_cast<TIME>(sqlite3_column_int64(Query_, column)); }

#endif
