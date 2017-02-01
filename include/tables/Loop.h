/**
 *
 *    @file  Loop.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_H_
#define  LOOP_H_

#include "Types.h"

/// @ingroup records
/// @brief Holds the information contained in one row of the _Loop_
/// database table.
struct loop_t {
    /// SQL ID of the loop.
    LOP_ID id;
    /// The line number.
    LIN_NO line_number;

    /// Constructor.
    constexpr explicit
    loop_t(LOP_ID sqlID,
           LIN_NO lineNumber) noexcept :
        id(sqlID),
        line_number(lineNumber)
    {}
};

#endif
