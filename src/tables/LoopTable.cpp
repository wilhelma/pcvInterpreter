/**
 *
 *    @file  LoopTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "LoopTable.h"

int LoopTable::fill(sqlite3_stmt *sqlstmt) {
   LOP_ID id   = static_cast<LOP_ID>(sqlite3_column_int(sqlstmt, 0));
   LIN_NO line = static_cast<LIN_NO>(sqlite3_column_int(sqlstmt, 1));

   loop_t *tmp = new loop_t(id, line);
   insert(id, *tmp);

   return 0;
}
