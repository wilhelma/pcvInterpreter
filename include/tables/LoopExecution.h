/**
 *
 *    @file  LoopExecution.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_EXECUTION_H_
#define  LOOP_EXECUTION_H_

#include "Types.h"

typedef struct loopExecution_t {
    LOE_ID sql_id;
    LOE_ID loop_id;
    unsigned parent_iteration;
    unsigned loop_duration;

    explicit
    loopExecution_t(LOE_ID sqlID,
                    LOE_ID loopID,
                    int parentIteration,
                    int loopDuration)
                    : sql_id(sqlID), loop_id(loopID),
                      parent_iteration(parentIteration), loop_duration(loopDuration)
    {}

} loopExecution_t;

#endif
