/**
 *
 *    @file  DBManager.h
 *   @brief  
 *
 *    @date  07/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  DBMANAGER_H_
#define  DBMANAGER_H_

#include "fwd/QueryResult.h"

#include <memory>
#include <sqlite3.h>
#include <string>

/// @defgroup database
/// @brief Classes related to the database operations.

/// @defgroup input
/// @brief Classes performing input operations from a database.
/// @ingroup database

/// @ingroup input
/// @brief Class to open a SQL database and query it.
class DBManager {
public:
    /// Constructor.
    explicit DBManager()
        : Database_(nullptr) //, CurrentQueryResult_(nullptr)
    {};

    /// @brief Try to open the database.
    /// @param DBPath The database to open.
    /// @throw SQLException
    void open(const std::string& DBPath);

    /// @brief Query the database.
    /// @param sql_query The database query string.
    /// @return A pointer to the QueryResult containing the result of the query.
    /// @throw SQLException
    std::unique_ptr<QueryResult> query(const std::string& sql_query) const;

    /// @brief Destructor: try to close the database.
    /// @attention If an exception is thrown when closing the database, the 
    /// execution will be `std::abort()`ed.
    ~DBManager() noexcept;

private:
    /// @brief Pointer to the opened database.
    /// @todo Use a `std::unique_ptr`. This fails because at compile time
    /// `sqlite3` appears to be an incomplete type. I've no idea why.
    sqlite3* Database_;
    
//  std::unique_ptr<sqlite3> Database_;

//  /// Pointer to the prepared statement currently being operated on
//  std::shared_ptr<QueryResult> CurrentQueryResult_
};

#endif
