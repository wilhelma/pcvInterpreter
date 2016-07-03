/**
 *
 *    @file  SegmentTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "SegmentTable.h"

const std::pair<SegmentTable::iterator, bool> SegmentTable::fill(sqlite3_stmt *sqlstmt) {
	SEG_ID id           = static_cast<SEG_ID>(sqlite3_column_int(sqlstmt, 0));
	CAL_ID call         = static_cast<CAL_ID>(sqlite3_column_int(sqlstmt, 1));
	SegmentType type    = static_cast<SegmentType>(sqlite3_column_int(sqlstmt, 2));
	LOI_ID loop_pointer = static_cast<LOI_ID>(sqlite3_column_int(sqlstmt, 3));

	segment_t *tmp = new segment_t(id, call, type, loop_pointer);
	return insert(id, *tmp);
}
