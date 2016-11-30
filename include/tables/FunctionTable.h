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
class FunctionTable final: public DBTable<FUN_ID, function_t> {
private:
    const size_type idToVectorIndex(const index_type& id) const final
    { return static_cast<size_type>(id) - 2; }
};

#endif
