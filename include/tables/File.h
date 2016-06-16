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

typedef struct file_t {
    FIL_ID sql_id;
    FIL_PT file_path;
    FIL_PT file_name; // retrieved from file_path

    explicit
    file_t(FIL_ID sqlID,
           FIL_PT filePath)
        : sql_id(sqlID), file_path(filePath)
    {
        retrieveFileName( filePath );
    }

    private:
    int retrieveFileName( FIL_PT filePath ) {
        // Separator
        const char directorySeparator = '/';
        // search for last occurrence
        size_t pos = filePath.find_last_of( directorySeparator );

        // if separator is not found
        if( pos > filePath.size() ) {
            this->file_name = filePath;
            return 1;
        }

        this->file_name = filePath.substr( pos );
        return 0;
    }
} file_t;

#endif
