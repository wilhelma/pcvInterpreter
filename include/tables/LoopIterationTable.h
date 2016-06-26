/**
 *
 *    @file  LoopIterationTable.h
 *   @brief 
 *
 *    @date  06/23/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_ITERATION_TABLE_H_
#define  LOOP_ITERATION_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "LoopIteration.h"
#include "Types.h"

class LoopIterationTable: public DBTable<LOI_ID, const loopIteration_t> {
	public:
		virtual int fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
