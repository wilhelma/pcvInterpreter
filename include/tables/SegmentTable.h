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

#include "fwd/Segment.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class SegmentTable final: public DBTable<SEG_ID, const segment_t> {
};

#endif
