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

#include <exception>

#include <memory>
#include <sqlite3.h>
#include <string>

/// @class DBManager
/// Class to manage the SQL database.
class DBManager {
public:
	/// Constructor.
	/// @param DBPath The path of the database
	explicit DBManager()
		: Database_(nullptr) //, CurrentQueryResult_(nullptr)
	{};

	/// Open the database.
	/// @param DBPath The database to open.
	void open(const std::string& DBPath);
	//
	/// Query the database.
	/// @param sql_query The database query string
	std::shared_ptr<QueryResult> query(const std::string& sql_query) const;

	/// Desctructor. Try to close the database.
	~DBManager();

private:
	/// Pointer to the opened database
	/// @todo Use a `std::unique_ptr`. This fails because at compile time
	/// `sqlite3` appears to be an incomplete type. I've no idea why.
//	std::unique_ptr<sqlite3> Database_;
	sqlite3* Database_;

//	/// Pointer to the prepared statement currently being operated on
//	std::shared_ptr<QueryResult> CurrentQueryResult_
};

#endif
