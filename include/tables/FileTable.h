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

#include "fwd/File.h"

#include "DBTable.h"
#include "Types.h"

/// @ingroup tables
class FileTable final: public DBTable<FIL_ID, const file_t> {
};

#endif
