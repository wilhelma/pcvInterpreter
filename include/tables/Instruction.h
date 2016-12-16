/**
 *
 *    @file  Instruction.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  INSTRUCTION_H_
#define  INSTRUCTION_H_

#include "Types.h"

/// @brief The type of the instruction.
enum class InstructionType : unsigned char {
    CALL    = 0, ///< Function call.
    ACCESS  = 1, ///< Memory access.
    ALLOC   = 2, ///< Memory allocation.
    FREE    = 3, ///< Memory free.
    FORK    = 4, ///< Process fork.
    JOIN    = 5, ///< Process join.
    ACQUIRE = 6, ///< Resurce acquisition.
    RELEASE = 7  ///< Resurce release.
};

/// @ingroup records
/// @brief Holds the information contained in one row of the _Instruction_
/// database table.
struct instruction_t {
    /// SQL ID of the instruction.
    INS_ID id;
    /// The ID of the segment containing the instruction.
    SEG_ID segment_id;
    /// The instruction type.
    InstructionType instruction_type;
    /// @brief The line number of the instruction.
    /// @todo To retrieve the file name?
    LIN_NO line_number;

    /// Constructor.
    explicit
    instruction_t(INS_ID instructionId,
                  SEG_ID segmentId,
                  InstructionType instructionType,
                  LIN_NO lineNumber) noexcept
                  : id(instructionId), segment_id(segmentId),
                  instruction_type(instructionType), line_number(lineNumber)
    {}
};

#endif
