/**
 *
 *    @file  FunctionTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FUNCTION_TABLE_H_
#define  FUNCTION_TABLE_H_

#include "fwd/Function.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class FunctionTable final: public DBTable<FUN_ID, const function_t> {
};

#endif
