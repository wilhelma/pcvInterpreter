/**
 *
 *    @file  SegmentTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SEGMENT_TABLE_H_
#define  SEGMENT_TABLE_H_

#include "DBTable.h"
#include "Segment.h"
#include "Types.h"

class SegmentTable: public DBTable<SEG_ID, const segment_t> {
	public:
		virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
