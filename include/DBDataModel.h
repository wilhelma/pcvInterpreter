/**
 * @file DBDataModel.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef DBDATAMODEL_H_
#define DBDATAMODEL_H_

#include <boost/serialization/strong_typedef.hpp>

#include <cstring>
#include <string>
#include <iostream>
#include "DataModel.h"
#include "Types.h"

typedef struct segment_t {
    SEG_ID sql_id;
    CAL_ID call_id;
    SegmentType segment_type;
    LOI_ID loop_pointer;

    explicit
    segment_t(SEG_ID sqlID,
              CAL_ID callId,
              SegmentType segmentType,
              LOI_ID loopPointer)
        : sql_id(sqlID), call_id(callId),
          segment_type(segmentType),
          loop_pointer(loopPointer)
    {
    }

} segment_t;

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


#endif /* DBDATAMODEL_H_ */
