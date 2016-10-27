/// @file   AccessTable.h
/// @brief  
/// @date   24/09/16
/// @author Paolo Di Giglio (github.com/pdigiglio),
///         <p.digiglio91@gmail.com>

#ifndef  FWD_ACCESS_TABLE_H_
#define  FWD_ACCESS_TABLE_H_

#include "Types.h"

#include <map>
#include <vector>

class AccessTable;

/// Vector of access IDs.
using AccessVector                  = std::vector<ACC_ID>;
/// Map an instruction ID to the IDs of its memory accesses.
using InstructionToAccessaVectorMap = std::map<INS_ID, AccessVector>;

#endif
