/**
 *
 *    @file  AccessTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_TABLE_H_
#define  ACCESS_TABLE_H_

#include <sqlite3.h>
#include <vector>

#include "Access.h"
#include "DBTable.h"
#include "Types.h"

class AccessTable: public DBTable<ACC_ID, const access_t> {
	public:
		virtual int fill(sqlite3_stmt *sqlstmt) override final;

		typedef std::vector<ACC_ID> accessVector_t;
		typedef std::map<INS_ID, accessVector_t> insAccessMap_t;

		const insAccessMap_t& getInsAccessMap() const { return _insAccessMap; }; 
	private:
		insAccessMap_t _insAccessMap;
};

#endif
