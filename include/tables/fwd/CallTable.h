/// @file   CallTable.h
/// @brief  
/// @date   24/09/16
/// @author Paolo Di Giglio (github.com/pdigiglio),
///         <p.digiglio91@gmail.com>


#ifndef  FWD_CALL_TABLE_H_
#define  FWD_CALL_TABLE_H_

#include "Types.h"
#include <map>

class CallTable;

/// Map between the instruction ID on which the call was issued and the issued call ID.
using InstructionToCallMap = std::map<INS_ID, CAL_ID>;

#endif
