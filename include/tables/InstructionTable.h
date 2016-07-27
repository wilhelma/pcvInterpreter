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

class InstructionTable final: public DBTable<INS_ID, const instruction_t> {
//	public:
//		virtual const std::pair<InstructionTable::iterator, bool> insert(const instruction_t& entry) override final
//		{ return map_.insert(typename std::map<INS_ID, const instruction_t>::value_type(entry.id, entry)); }
//
//		virtual InstructionTable::iterator insert(InstructionTable::iterator hint, const instruction_t& entry) override final
//		{ return map_.insert(hint, typename std::map<INS_ID, const instruction_t>::value_type(entry.id, entry)); }
};

#endif
