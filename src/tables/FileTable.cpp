/**
 *
 *    @file  FileTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "FileTable.h"

const std::pair<FileTable::iterator, bool> FileTable::fill(sqlite3_stmt *sqlstmt) {
	FIL_ID id        = static_cast<FIL_ID>(sqlite3_column_int(sqlstmt, 0));
	FIL_PT file_path(reinterpret_cast<const char*>(sqlite3_column_text(sqlstmt, 1)));

	file_t *tmp = new file_t(id, file_path);
	return insert(id, *tmp);    
}
