/**
 *
 *    @file  CallTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_TABLE_H_
#define  CALL_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "Call.h"
#include "Types.h"

class CallTable: public DBTable<CAL_ID, const call_t> {
	public:
		virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
