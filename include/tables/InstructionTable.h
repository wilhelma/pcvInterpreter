/**
 *
 *    @file  InstructionTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  INSTRUCTION_TABLE_H_
#define  INSTRUCTION_TABLE_H_

#include "DBTable.h"
#include "Instruction.h"
#include "Types.h"

/// @ingroup tables
class InstructionTable final: public DBTable<INS_ID, const instruction_t> {
};

#endif
