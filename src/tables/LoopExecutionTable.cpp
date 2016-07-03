/**
 *
 *    @file  LoopExecutionTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "LoopExecutionTable.h"

const std::pair<LoopExecutionTable::iterator, bool> LoopExecutionTable::fill(sqlite3_stmt *sqlstmt) {
    LOE_ID id    = static_cast<LOE_ID>(sqlite3_column_int(sqlstmt, 0));
    LOE_ID loop  = static_cast<LOE_ID>(sqlite3_column_int(sqlstmt, 1));
    int parent   = sqlite3_column_int(sqlstmt, 2);
    int duration = sqlite3_column_int(sqlstmt, 3);

    loopExecution_t *tmp = new loopExecution_t(id, loop, parent, duration);
    return insert(id, *tmp);
}

