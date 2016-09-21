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

/// @ingroup records
/// @brief Holds the information contained in one row of the _LoopIteration_
/// database table.
struct loopIteration_t {
    /// SQL ID of the file.
    LOI_ID id;
    /// ID of the loop execution.
    LOE_ID loop_execution;
    /// ID of the loop iteration.
    LOI_ID loop_iteration;

    /// Constructor.
    explicit loopIteration_t(LOI_ID sqlID,
                             LOE_ID loopExecution,
                             LOI_ID loopIteration) noexcept :
        id(sqlID), loop_execution(loopExecution),
        loop_iteration(loopIteration)
        {}
};

#endif
