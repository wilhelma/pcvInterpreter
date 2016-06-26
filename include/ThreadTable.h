/**
 *
 *    @file  ThreadTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  THREAD_TABLE_H_
#define  THREAD_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "Thread.h"
#include "Types.h"

class ThreadTable: public DBTable<TRD_ID, const thread_t> {
	public:
		virtual int fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
