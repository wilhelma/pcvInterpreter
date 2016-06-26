/**
 *
 *    @file  LoopExecutionTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_EXECUTION_TABLE_H_
#define  LOOP_EXECUTION_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "LoopExecution.h"
#include "Types.h"

class LoopExecutionTable: public DBTable<LOE_ID, const loopExecution_t> {
	public:
		virtual int fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
