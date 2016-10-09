/**
 *
 *    @file  FileTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FILE_TABLE_H_
#define  FILE_TABLE_H_

#include "DBTable.h"
#include "File.h"
#include "Types.h"

/// @ingroup tables
class FileTable final: public DBTable<FIL_ID, const file_t> {
//	public:
//		virtual const std::pair<FileTable::iterator, bool> insert(const file_t& entry) override final
//		{ return map_.insert(typename std::map<FIL_ID, const file_t>::value_type(entry.id, entry)); }
//
//		virtual FileTable::iterator insert(FileTable::iterator hint, const file_t& entry) override final
//		{ return map_.insert(hint, typename std::map<FIL_ID, const file_t>::value_type(entry.id, entry)); }
};

#endif
