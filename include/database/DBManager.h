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
    constexpr explicit DBManager() noexcept
        : Database_(nullptr)
    {}

    /// @brief Try to open the database.
    /// @param DBPath The database to open.
    /// @throw SQLException
    void open(const std::string& DBPath);

    /// @brief Query the database.
    /// @param sql_query The database query string.
    /// @return A pointer to the QueryResult containing the result of the query.
    /// @throw SQLException If the query fails.
    std::unique_ptr<const QueryResult> query(const std::string& sql_query) const;

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
};

/// @brief Query a table from the database
/// @param table_name The name of the table to query for.
/// @param connection The connection through which access the database.
/// @param offset     The entries to skip.
/// @param entries    The number of entries to query for.
/// @throw SQLException If the query fails.
inline std::unique_ptr<const QueryResult> query_table(const std::string& table_name, const DBManager& connection, const size_t offset, const size_t entries)
{ return connection.query("select * from " + table_name + " limit " + std::to_string(entries) + " offset " + std::to_string(offset)); }

/// @brief Returns the number of entries in a database table.
/// @param table_name The name of the table.
/// @param connection The connection through which access the database.
/// @throw SQLException If the `count(*)` query fails.
const size_t entries(const std::string& table_name, const DBManager& connection);

/// @brief Connects to a database.
/// @param DBPath The database to connect to.
const DBManager open_database_connection(const std::string& DBPath) noexcept;

#endif
