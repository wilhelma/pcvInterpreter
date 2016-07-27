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

class ThreadTable final: public DBTable<TRD_ID, const thread_t> {
//	public:
//		virtual const std::pair<iterator, bool> fill(const SQLStatementIterator& i) override final
//		{ return insert(i.operator*<thread_t>()); }
};

#endif
