/**
 *
 *    @file  File.cpp
 *   @brief  
 *
 *    @date  09/07/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "File.h"

#include "Types.h"

// helper function to retrieve the file name from its path
decltype(auto) retrieveFileName(FIL_PT filePath, const char directorySeparator = '/') {
    // search for last occurrence of the directory separator
    auto pos = filePath.find_last_of(directorySeparator);
    return ((pos > filePath.size()) ? filePath : filePath.substr(pos));
}

file_t::file_t(FIL_ID sqlID, FIL_PT filePath) noexcept
    : id(sqlID), file_path(filePath), file_name(retrieveFileName(filePath))
{}
