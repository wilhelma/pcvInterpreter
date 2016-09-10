/**
 *
 *    @file  Segment.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SEGMENT_iNC
#define  SEGMENT_iNC

#include "Types.h"

/// @ingroup types
/// @brief The type of the segment.
enum class SegmentType : bool {
    REGION = 0, ///< @todo ?
    LOOP   = 1  ///< @todo ?
};

/// @ingroup records
/// @brief Holds the information contained in one row of the _Segment_
/// database table.
/// @todo document this and clarify what is a segment.
struct segment_t {
    /// SQL ID of the segment.
    SEG_ID id;
    CAL_ID call_id;
    SegmentType segment_type;
    LOI_ID loop_pointer;

    explicit
    segment_t(SEG_ID sqlID,
              CAL_ID callId,
              SegmentType segmentType,
              LOI_ID loopPointer) noexcept
        : id(sqlID), call_id(callId),
          segment_type(segmentType),
          loop_pointer(loopPointer)
    {
    }

};

#endif
