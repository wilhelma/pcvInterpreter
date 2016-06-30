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

typedef struct thread_t {
    TRD_ID id;
    TIME_STRING start_time;
    TIME_STRING end_time;
    NUM_CYCLES num_cycles;
    INS_ID create_instruction_id;
    INS_ID join_instruction_id;
    TRD_ID parent_thread_id;
    PID process_id;

    explicit
    thread_t(TRD_ID id,
             TIME_STRING start_time,
             TIME_STRING end_time,
             NUM_CYCLES num_cycles,
             INS_ID createINS_id,
             INS_ID joinIns_id,
             TRD_ID parentThreadId,
             PID processId)
        : id(id), start_time(start_time), end_time(end_time),
          num_cycles(num_cycles), create_instruction_id(createINS_id),
          join_instruction_id(joinIns_id), parent_thread_id(parentThreadId),
          process_id(processId) {}
} thread_t;

#endif
