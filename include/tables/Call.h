/**
 *
 *    @file  Call.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  call_H_
#define  call_H_

#include "Types.h"

/// @ingroup records
/// @brief Holds the information contained in one row of the _Call_
/// database table.
struct call_t {
    static const CAL_ID MAIN;// = static_cast<CAL_ID>(1);

    /// SQL ID of the call.
    CAL_ID id;
    /// ID of the thread where the function was called from.
    TRD_ID thread_id;
    /// ID of the called function.
    FUN_ID function_id;
    /// ID of the instruction where the function was called from.
    INS_ID instruction_id;
    /// Time when the call started.
    TIME start_time;
    /// Time when the call ended (i.e. the called function returned).
    TIME end_time;

    /// Constructor.
    explicit
    call_t(CAL_ID sqlID,
           TRD_ID threadID,
           FUN_ID functionID,
           INS_ID instructionID,
           TIME startTime,
           TIME endTime) noexcept
        : id(sqlID), thread_id(threadID),
          function_id(functionID), instruction_id(instructionID),
          start_time(startTime), end_time(endTime)
    {
    }
};

#endif
