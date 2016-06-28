/**
 *
 *    @file  ReferenceTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ReferenceTable.h"

const std::pair<ReferenceTable::iterator, bool> ReferenceTable::fill(sqlite3_stmt *sqlstmt) {
    REF_ID id          = static_cast<REF_ID>(sqlite3_column_int(sqlstmt, 0));
    REF_SIZE size      = static_cast<REF_SIZE>(sqlite3_column_int(sqlstmt, 1));
    ReferenceType type = static_cast<ReferenceType>(sqlite3_column_int(sqlstmt, 2));
    INS_ID allocinstr  = static_cast<INS_ID>(sqlite3_column_int(sqlstmt, 4));
    REF_NAME name(reinterpret_cast<const char*>(sqlite3_column_text(sqlstmt, 3)));

    reference_t *tmp = new reference_t(id, size, type, name, allocinstr);
    return insert(id, *tmp);
}
