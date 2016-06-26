/**
 *
 *    @file  AccessTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AccessTable.h"

int AccessTable::fill(sqlite3_stmt *sqlstmt) {
    ACC_ID id                = static_cast<ACC_ID>(sqlite3_column_int(sqlstmt, 0));
    INS_ID instruction_id    = static_cast<INS_ID>(sqlite3_column_int(sqlstmt, 1));
    POS position             = static_cast<POS>(sqlite3_column_int(sqlstmt, 2));
    REF_ID reference_id      = static_cast<REF_ID>(sqlite3_column_int(sqlstmt, 3));
    AccessType access_type   = static_cast<AccessType>(sqlite3_column_int(sqlstmt, 4));
    AccessState memory_state = static_cast<AccessState>(sqlite3_column_int(sqlstmt, 5));

    access_t *tmp = new access_t(id, instruction_id, position, reference_id, access_type, memory_state); 

    insert(id, *tmp);         
    _insAccessMap[instruction_id].push_back(id); // create 1:n associations 
    return 0;
}
