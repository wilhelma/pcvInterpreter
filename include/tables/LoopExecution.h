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

/// @ingroup records
/// @brief Holds the information contained in one row of the _LoopExecution_
/// database table.
/// @todo Document this!
struct loopExecution_t {
    /// SQL ID of the loop execution.
    LOE_ID id;
    /// The ID of the loop.
    LOE_ID loop_id;
    unsigned parent_iteration;
    unsigned loop_duration;

    /// Constructor
    explicit
    loopExecution_t(LOE_ID sqlID,
                    LOE_ID loopID,
                    int parentIteration,
                    int loopDuration) noexcept
        : id(sqlID), loop_id(loopID),
        parent_iteration(parentIteration), loop_duration(loopDuration)
    {}

};

#endif
