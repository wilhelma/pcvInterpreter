/**
 *
 *    @file  LoopTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_TABLE_H_
#define  LOOP_TABLE_H_

#include "fwd/Loop.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class LoopTable final: public DBTable<LOP_ID, loop_t> {
};

#endif
