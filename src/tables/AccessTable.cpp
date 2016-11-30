/**
 *
 *    @file  AccessTable.cpp
 *   @brief  
 *
 *    @date  10/27/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AccessTable.h"

#include "Access.h"
#include "Types.h"

#include <vector>

AccessTable::iterator AccessTable::insert(AccessTable::iterator hint, access_t&& entry) {
    InstructionToAccessaVectorMap_[entry.instruction_id].push_back(entry.id); // create 1:n associations 
    return Vector_.emplace(hint, std::forward<access_t>(entry));
}
