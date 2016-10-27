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

#include <map>
#include <vector>

const std::pair<AccessTable::iterator, bool> AccessTable::insert(const access_t& entry) {
    InstructionToAccessaVectorMap_[entry.instruction_id].push_back(entry.id); // create 1:n associations 
    return Map_.insert(typename std::map<ACC_ID, const access_t>::value_type(entry.id, entry));
}

AccessTable::iterator AccessTable::insert(AccessTable::iterator hint, const access_t& entry) {
    InstructionToAccessaVectorMap_[entry.instruction_id].push_back(entry.id); // create 1:n associations 
    return Map_.insert(hint, typename std::map<ACC_ID, const access_t>::value_type(entry.id, entry));
}
