/**
 *
 *    @file  LoopTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_TABLE_H_
#define  LOOP_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "Loop.h"
#include "Types.h"

class LoopTable: public DBTable<LOP_ID, loop_t> {
	public:
		virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
