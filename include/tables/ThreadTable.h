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

#include "fwd/Thread.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class ThreadTable final: public DBTable<TRD_ID, thread_t> {
private:
    const size_type idToVectorIndex(const index_type& id) const final
    { return static_cast<size_type>(id); }
};

#endif
