/**
 *
 *    @file  FunctionTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "FunctionTable.h"

const std::pair<FunctionTable::iterator, bool> FunctionTable::fill(sqlite3_stmt *sqlstmt) {
    FUN_ID id          = static_cast<FUN_ID>(sqlite3_column_int(sqlstmt, 0));
    FUN_SG signature(reinterpret_cast<const char*>(sqlite3_column_text(sqlstmt, 2)));
    FIL_ID file_id     = static_cast<FIL_ID>(sqlite3_column_int(sqlstmt, 3));
    LIN_NO line_number = static_cast<LIN_NO>(sqlite3_column_int(sqlstmt, 4));
    FunctionType type  = static_cast<FunctionType>(sqlite3_column_int(sqlstmt, 6));

   function_t *tmp = new function_t(id, signature, type, file_id, line_number);
   return insert(id, *tmp);
}

