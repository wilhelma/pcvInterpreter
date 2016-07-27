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

#include "DBTable.h"
#include "Reference.h"
#include "Types.h"

class ReferenceTable final: public DBTable<REF_ID, const reference_t> {
};

#endif
