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

struct call_t {
    static const CAL_ID MAIN;// = static_cast<CAL_ID>(1);

    CAL_ID id;
    TRD_ID thread_id;
    FUN_ID function_id;
    INS_ID instruction_id;
    TIME start_time;
    TIME end_time;

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
