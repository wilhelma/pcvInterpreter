/**
 *
 *    @file  AccessTable.h
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_TABLE_H_
#define  ACCESS_TABLE_H_

#include "fwd/Access.h"
#include "fwd/AccessTable.h"

#include "DBTable.h"
#include "Types.h"

#include <map>
#include <vector>

/// @ingroup tables
/// @brief Table of _Accesses_.
/// @details It also keeps a map between instruction IDs and their respective memory accesses.
class AccessTable final: public DBTable<ACC_ID, access_t> {
public:

    /// @brief Inserts entry in the position before _hint_.
    /// @param hint  The iterator to the insertion position.
    /// @param entry The element to insert.
    virtual AccessTable::iterator insert(AccessTable::iterator hint, access_t&& entry) final;

    /// Returns the internal map between instruction IDs and access IDs.
    const InstructionToAccessaVectorMap& getInsAccessMap() const noexcept
    { return InstructionToAccessaVectorMap_; }; 

private:
    /// Map an instruction ID to the IDs of its memory accesses.
    InstructionToAccessaVectorMap InstructionToAccessaVectorMap_;
};

#endif
