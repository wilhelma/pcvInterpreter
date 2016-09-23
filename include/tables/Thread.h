/**
 *
 *    @file  Thread.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */


#ifndef  THREAD_H_
#define  THREAD_H_

#include "Types.h"

static const TRD_ID NO_TRD_ID = static_cast<TRD_ID>(NO_ID);

/// @ingroup records
/// @brief Holds the information contained in one row of the _Thread_
/// database table.
/// @todo document this
struct thread_t {
    /// SQL ID of the thread.
    TRD_ID id;
    TIME_STRING start_time;
    TIME_STRING end_time;
    TIME start_cycle;
    TIME num_cycles;
    INS_ID create_instruction_id;
    INS_ID join_instruction_id;
    TRD_ID parent_thread_id;
    PID process_id;
    CAL_ID call_id;

    /// Constructor
    explicit thread_t(TRD_ID id,
                      TIME_STRING start_time,
                      TIME_STRING end_time,
                      TIME start_cycle,
                      TIME num_cycles,
                      INS_ID createINS_id,
                      INS_ID joinIns_id,
                      TRD_ID parentThreadId,
                      PID processId,
                      CAL_ID callId) noexcept :
        id(id), start_time(start_time), end_time(end_time),
        start_cycle(start_cycle), num_cycles(num_cycles),
        create_instruction_id(createINS_id), join_instruction_id(joinIns_id),
        parent_thread_id(parentThreadId), process_id(processId),
        call_id(callId)
        {}
};

#endif
