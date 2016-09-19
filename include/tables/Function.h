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

#include "Types.h"

/// @brief The type of the function.
/// @todo document this!
enum class FunctionType : uint32_t  {
	ENTRY_ROUTINE    = (1 << 0),  // 1
	EXIT_ROUTINE     = (1 << 1),  // 2
	FUNCTION         = (1 << 2),  // 4
	METHOD           = (1 << 3),  // 8
	FREE             = (1 << 4),  // ...
	FORK             = (1 << 5),
	JOIN             = (1 << 6),
	ACQUIRE          = (1 << 7),
	RELEASE          = (1 << 8),
	EXTERNAL         = (1 << 9),
	ALLOC            = (1 << 10),
	BARRIER          = (1 << 11),
	WAIT             = (1 << 12),
	SIGNAL_SINGLE    = (1 << 13),
	SIGNAL_BROADCAST = (1 << 14),
};

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
    explicit
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
