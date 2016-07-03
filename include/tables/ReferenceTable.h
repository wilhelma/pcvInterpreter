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

#include <sqlite3.h>

#include "DBTable.h"
#include "Reference.h"
#include "Types.h"

class ReferenceTable: public DBTable<REF_ID, const reference_t> {
	public:
		virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) override final;
};

#endif
