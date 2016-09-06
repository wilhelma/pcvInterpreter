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

#include "DBTable.h"
#include "Function.h"
#include "Types.h"

class FunctionTable final: public DBTable<FUN_ID, const function_t> {
//	public:
//		virtual const std::pair<FunctionTable::iterator, bool> insert(const function_t& entry) override final
//		{ return map_.insert(typename std::map<FUN_ID, const function_t>::value_type(entry.id, entry)); }
//
//		virtual FunctionTable::iterator insert(FunctionTable::iterator hint, const function_t& entry) override final
//		{ return map_.insert(hint, typename std::map<FUN_ID, const function_t>::value_type(entry.id, entry)); }
};

#endif
