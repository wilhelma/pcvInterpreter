/**
 *
 *    @file  CallTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_TABLE_H_
#define  CALL_TABLE_H_

#include "Call.h"
#include "DBTable.h"
#include "Types.h"

class CallTable final: public DBTable<CAL_ID, const call_t> {
};

#endif
