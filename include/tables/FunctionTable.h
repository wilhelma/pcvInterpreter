/**
 *
 *    @file  FunctionTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FUNCTION_TABLE_H_
#define  FUNCTION_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "Function.h"
#include "Types.h"

class FunctionTable: public DBTable<FUN_ID, const function_t> {
	public:
		virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
