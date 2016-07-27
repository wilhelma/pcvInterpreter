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

#include "DBTable.h"
#include "Loop.h"
#include "Types.h"

class LoopTable final: public DBTable<LOP_ID, const loop_t> {
};

#endif
