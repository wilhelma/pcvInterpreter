/**
 *
 *    @file  Access.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_H_
#define  ACCESS_H_

#include "Types.h"

/// @brief The type of the access.
enum class AccessType : unsigned char {
	READ  = 1, ///< Read access.
	WRITE = 2  ///< Write access.
};

/// @brief The state of the access.
/// @todo What are these states?
enum class AccessState : unsigned char {
    INIT              = 0,
    EXCLUSIVE         = 1,
    READ_SHARED       = 2,
    READ_WRITE_SHARED = 3
};

/// @defgroup records
/// @brief Structures holding data read from the database.
/// @todo The naming of the struct member variables is not consistent.
/// @ingroup database

/// @ingroup records
/// @brief Holds the information contained in one row of the _Access_
/// database table.
struct access_t {
    /// SQL ID of the access.
    ACC_ID id;
    /// ID of the instruction where the memory was accessed from.
    INS_ID instruction_id;
    /// @todo position?
    POS position;
    /// @todo reference_id?
    REF_ID reference_id;
    /// The type of the access.
    AccessType access_type;
    /// The state of the access.
    AccessState memory_state;

    /// Constructor.
    explicit
    access_t(ACC_ID sqlID,
             INS_ID instructionID,
             POS pos,
             REF_ID referenceID,
             AccessType accessType,
             AccessState memoryState) noexcept
        : id(sqlID), instruction_id(instructionID), position(pos),
          reference_id(referenceID), access_type(accessType),
          memory_state(memoryState)
    {}
};

#endif
