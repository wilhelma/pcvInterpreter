/**
 *
 *    @file  AccessInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_INFO_H_
#define  ACCESS_INFO_H_

#include "fwd/Reference.h"

#include "Access.h" // Contains the AccessType!
#include "ShadowVariableMap.h"
#include "Types.h"

#include <memory>

/// @defgroup info
/// @brief Information about various runtime events.

/// @ingroup info
/// @brief Information about a memory access.
/// @todo Find a way to remove the entry from the ShadowMap after the usage
class AccessInfo {
public:
	/// @brief Constructor.
    /// @param var_info       The information about the accessed variable.
    /// @param instruction_id The ID of the instruction where the variable was accessed from.
    /// @param access_type    The access type.
	explicit
	AccessInfo(ShadowVariableMap::const_iterator var_info,
               const INS_ID& instruction_id,
               const AccessType access_type) noexcept
        : InstructionId_(instruction_id),
          AccessType_(access_type),
          Variable_(var_info)
        {}

    /// Return the ID of the instruction that accessed the variable.
    const INS_ID& instructionId() const noexcept
    { return InstructionId_; }

    /// Return the ID of the accessed variable.
    const REF_ID& variableId() const noexcept;

    /// Return the access type.
    const AccessType accessType() const noexcept;

    /// Return the variable type (_heap_, _stack_,...).
    const ReferenceType referenceType() const noexcept;
    
    /// Return the variable size.
    const REF_SIZE& size() const noexcept;

    /// Return the variable name.
    const REF_NAME& name() const noexcept;

private:
	/// ID of the accessing instruction.
	INS_ID InstructionId_;

    /// The access type.
    AccessType AccessType_;

    /// The variable being accessed.
    ShadowVariableMap::const_iterator Variable_;
};

#endif
