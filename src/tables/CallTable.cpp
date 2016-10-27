/**
 *
 *    @file  CallTable.cpp
 *   @brief  
 *
 *    @date  10/27/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "CallTable.h"

#include "Call.h"
#include "Instruction.h"
#include "Types.h"

const std::pair<CallTable::iterator, bool> CallTable::insert(const call_t& entry) {
    InstructionToCall_.insert(typename std::map<INS_ID, CAL_ID>::value_type(entry.instruction_id, entry.id));
    return Map_.insert(typename std::map<CAL_ID, const call_t>::value_type(entry.id, entry));
}

CallTable::iterator CallTable::insert(CallTable::iterator hint, const call_t& entry) {
    InstructionToCall_.insert(typename std::map<INS_ID, CAL_ID>::value_type(entry.instruction_id, entry.id));
    return Map_.insert(hint, typename std::map<CAL_ID, const call_t>::value_type(entry.id, entry));
}
