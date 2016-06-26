/**
 *
 *    @file  CallTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "CallTable.h"

int CallTable::fill(sqlite3_stmt *sqlstmt) {
    CAL_ID id             = static_cast<CAL_ID>(sqlite3_column_int(sqlstmt, 0));
    TRD_ID thread_id      = static_cast<TRD_ID>(sqlite3_column_int(sqlstmt, 1));
    FUN_ID function_id    = static_cast<FUN_ID>(sqlite3_column_int(sqlstmt, 2));
    INS_ID instruction_id = static_cast<INS_ID>(sqlite3_column_int(sqlstmt, 3));
    TIME start_time       = static_cast<TIME>(sqlite3_column_int(sqlstmt, 4));
    TIME end_time         = static_cast<TIME>(sqlite3_column_int(sqlstmt, 5));

    call_t *tmp = new call_t(id, thread_id, function_id, instruction_id, start_time, end_time);

    insert(id, *tmp);       
    return 0;
}
