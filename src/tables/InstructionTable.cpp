/**
 *
 *    @file  InstructionTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "InstructionTable.h"

const std::pair<InstructionTable::iterator, bool> InstructionTable::fill(sqlite3_stmt *sqlstmt) {
   INS_ID id            = static_cast<INS_ID>(sqlite3_column_int(sqlstmt, 0));
   SEG_ID segment       = static_cast<SEG_ID>(sqlite3_column_int(sqlstmt, 1));
   InstructionType type = static_cast<InstructionType>(sqlite3_column_int(sqlstmt, 2));
   LIN_NO line          = static_cast<LIN_NO>(sqlite3_column_int(sqlstmt, 3));

   instruction_t *tmp = new instruction_t(id, segment, type, line);
   return insert(id, *tmp);
}
