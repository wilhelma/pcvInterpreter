/**
 *
 *    @file  FileTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FILE_TABLE_H_
#define  FILE_TABLE_H_

#include <sqlite3.h>

#include "DBTable.h"
#include "File.h"
#include "Types.h"

class FileTable: public DBTable<FIL_ID, const file_t> {
	public:
		virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
