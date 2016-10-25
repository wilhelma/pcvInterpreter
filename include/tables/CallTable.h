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

#include "Call.h"
#include "Instruction.h"
#include "DBTable.h"
#include "Types.h"

#include <map>

/// @ingroup tables
/// @brief Associates each call ID to the `call_t` that contains the information.
/// @details The class also contains a map between the instruction ID on which the call was
/// issued and the (issued) call ID.
class CallTable final: public DBTable<CAL_ID, const call_t> {
    public:
        virtual const std::pair<CallTable::iterator, bool> insert(const call_t& entry) override
        {
            InstructionToCall_.insert(typename std::map<INS_ID, CAL_ID>::value_type(entry.instruction_id, entry.id));
            return Map_.insert(typename std::map<CAL_ID, const call_t>::value_type(entry.id, entry));
        }

        virtual CallTable::iterator insert(CallTable::iterator hint, const call_t& entry) override
        {
            InstructionToCall_.insert(typename std::map<INS_ID, CAL_ID>::value_type(entry.instruction_id, entry.id));
            return Map_.insert(hint, typename std::map<CAL_ID, const call_t>::value_type(entry.id, entry));
        }

        const std::map<INS_ID, CAL_ID>& instructionToCall() const noexcept
        { return InstructionToCall_; }

    private:
        std::map<INS_ID, CAL_ID> InstructionToCall_;
};

#endif
