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

inline FIL_PT retrieve_file_name(const FIL_PT& path) noexcept {
    const char dir_separator = '/';

    // Index of the last non-terminator character in the string.
    int i = path.length() - 1;
    while (path[i] != dir_separator && i > 0)
        -- i;

    // Advance i by one to avoid the '/' in the file name
    i += (path[i] == dir_separator ? 1 : 0);
    return FastString<unsigned char>(path.c_str() + i);
}

/// @ingroup records
/// @brief Holds the information contained in one row of the _File_.
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
    explicit file_t(FIL_ID sqlID,
                    FIL_PT filePath) noexcept :
        id(sqlID),
        file_path(std::move(filePath)),
        file_name(std::move(retrieve_file_name(file_path)))
   {}
};

#endif
