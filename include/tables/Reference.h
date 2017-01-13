/**
 *
 *    @file  Reference.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  REFERENCE_H_
#define  REFERENCE_H_

#include "ReferenceType.h"
#include "Types.h"

/// @ingroup records
/// @brief Holds the information contained in one row of the _Reference_
/// database table.
/// @todo remove std::string and make constexpr
struct reference_t {
    /// SQL ID of the reference.
    REF_ID id;
    /// @brief Size of the reference.
    /// @todo In bytes, bits, words, kilograms?
    REF_SIZE size;
    /// The reference type.
    ReferenceType memory_type;
    /// The reference name.
    REF_NAME name;
    /// @brief ID of the instruction that allocated the memory.
    /// @todo Is this correct?
    INS_ID allocinstr;

    /// Constructor.
    explicit
    reference_t(REF_ID refId,
                REF_SIZE refSize,
                ReferenceType memoryType,
                REF_NAME refName,
                INS_ID allocInstr) noexcept
        : id(refId), size(refSize), memory_type(memoryType),
          name(refName), allocinstr(allocInstr)
    {}
};

#endif
