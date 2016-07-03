/**
 *
 *    @file  LoopIterationTable.cpp
 *   @brief  
 *
 *    @date  06/23/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "LoopIterationTable.h"

const std::pair<LoopIterationTable::iterator, bool> LoopIterationTable::fill(sqlite3_stmt *sqlstmt) {
    LOI_ID id            = static_cast<LOI_ID>(sqlite3_column_int(sqlstmt, 0));
    LOE_ID loopExecution = static_cast<LOE_ID>(sqlite3_column_int(sqlstmt, 1));
    LOI_ID loopIteration = static_cast<LOI_ID>(sqlite3_column_int(sqlstmt, 2));

    return insert(id, loopIteration_t(id, loopExecution, loopIteration));
}
