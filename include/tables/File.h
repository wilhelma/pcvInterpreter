/**
 *
 *    @file  File.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FILE_H_
#define  FILE_H_

#include "Types.h"

/// @ingroup records
/// @brief Holds the information contained in one row of the _File_
/// database table.
struct file_t {
    /// SQL ID of the file.
    FIL_ID id;
    /// The file path (and name).
    FIL_PT file_path;
    /// The file name only (retrieved from the file path).
    FIL_PT file_name;

    /// Constructor.
    /// @param sqlID    The SQL ID of the file.
    /// @param filePath The file path and name.
    explicit file_t(FIL_ID sqlID, FIL_PT filePath) noexcept;
};

#endif
