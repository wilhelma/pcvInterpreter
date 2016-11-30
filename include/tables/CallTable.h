/**
 *
 *    @file  CallTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_TABLE_H_
#define  CALL_TABLE_H_

#include "fwd/Call.h"
#include "fwd/CallTable.h"
#include "fwd/Instruction.h"

#include "DBTable.h"
#include "Types.h"

#include <map>

/// @ingroup tables
/// @brief Associates each call ID to the `call_t` that contains the information.
/// @details The class also contains a map between the instruction ID on which the call was
/// issued and the issued call ID.
class CallTable final: public DBTable<CAL_ID, call_t> {
public:

    /// @brief Inserts entry in the position _hint_.
    /// @param hint  The iterator to the insertion position.
    /// @param entry The element to insert.
    virtual CallTable::iterator insert(CallTable::iterator hint, call_t&& entry) final;

    /// Returns the internal map between the instruction ID and the call ID.
    const InstructionToCallMap& instructionToCall() const noexcept
    { return InstructionToCall_; }

private:
    /// Map between the instruction ID on which the call was issued and the issued call ID.
    InstructionToCallMap InstructionToCall_;
};

#endif
