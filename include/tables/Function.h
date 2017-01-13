/**
 *
 *    @file  Function.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FUNCTION_H_
#define  FUNCTION_H_

#include <iostream>

#include "FunctionType.h"
#include "Types.h"

/// @ingroup records
/// @brief Holds the information contained in one row of the _Function_
/// database table.
struct function_t {
    /// SQL ID of the function.
    FUN_ID id;
    /// The Function name (_it's not the signature_).
    FUN_NM name;
    /// The Function signature.
    FUN_SG signature;
    /// The function type.
    FunctionType type;
    /// @brief The ID of the file of the function.
    /// @todo Which file? The one where it was defined or
    /// the one where it was declared?
    FIL_ID file_id;
    /// @todo Line number of what?
    LIN_NO line_number;

    /// Constructor.
    constexpr explicit
    function_t(FUN_ID sqlID,
               FUN_NM name,
               FUN_SG fnSignature,
               FunctionType fnType,
               FIL_ID fileId,
               LIN_NO lineNumber) noexcept
        : id(sqlID), name(name), signature(fnSignature), type(fnType),
          file_id(fileId), line_number(lineNumber)
    {}
};

#endif
