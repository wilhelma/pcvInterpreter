/**
 *
 *    @file  LoopIteration.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_ITERATION_H_
#define  LOOP_ITERATION_H_

#include "Types.h"

struct loopIteration_t {
    LOI_ID id;
    LOE_ID loop_execution;
    LOI_ID loop_iteration;

    explicit
    loopIteration_t(LOI_ID sqlID,
                    LOE_ID loopExecution,
                    LOI_ID loopIteration) noexcept
                    : id(sqlID),
                      loop_execution(loopExecution),
                      loop_iteration(loopIteration)
    {}
};

#endif
