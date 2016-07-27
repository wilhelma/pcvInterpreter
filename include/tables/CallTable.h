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
//	public:
//		virtual const std::pair<CallTable::iterator, bool> insert(const call_t& entry) override final
//		{ return map_.insert(typename std::map<CAL_ID, const call_t>::value_type(entry.id, entry)); }
//
//		virtual CallTable::iterator insert(CallTable::iterator hint, const call_t& entry) override final
//		{ return map_.insert(hint, typename std::map<CAL_ID, const call_t>::value_type(entry.id, entry)); }
};

#endif
