/**
 *
 *    @file  LoopExecutionTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_EXECUTION_TABLE_H_
#define  LOOP_EXECUTION_TABLE_H_

#include "fwd/LoopExecution.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class LoopExecutionTable final: public DBTable<LOE_ID, const loopExecution_t> {
};

#endif
