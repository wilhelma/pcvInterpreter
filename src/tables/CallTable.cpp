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

#include <vector>

CallTable::iterator CallTable::insert(CallTable::iterator hint, call_t&& entry) {
    InstructionToCall_.insert(typename std::map<INS_ID, CAL_ID>::value_type(entry.instruction_id, entry.id));
    return Vector_.emplace(hint, std::forward<call_t>(entry));
}
