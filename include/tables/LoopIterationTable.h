/**
 *
 *    @file  LoopIterationTable.h
 *   @brief 
 *
 *    @date  06/23/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_ITERATION_TABLE_H_
#define  LOOP_ITERATION_TABLE_H_

#include "DBTable.h"
#include "LoopIteration.h"
#include "Types.h"

/// @ingroup tables
class LoopIterationTable final: public DBTable<LOI_ID, const loopIteration_t> {
};

#endif
