/**
 *
 *    @file  ThreadTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  THREAD_TABLE_H_
#define  THREAD_TABLE_H_

#include "DBTable.h"
#include "Thread.h"
#include "Types.h"

/// @ingroup tables
class ThreadTable final: public DBTable<TRD_ID, const thread_t> {
};

#endif
