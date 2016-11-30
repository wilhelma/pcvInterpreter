/**
 *
 *    @file  ReferenceTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  REFERENCE_TABLE_H_
#define  REFERENCE_TABLE_H_

#include "fwd/Reference.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class ReferenceTable final: public DBTable<REF_ID, reference_t> {
};

#endif
