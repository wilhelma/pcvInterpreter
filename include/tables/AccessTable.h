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

#include "Access.h"
#include "DBTable.h"
#include "Types.h"

#include <map>
#include <vector>

/// @ingroup tables
/// @brief Table of `access_t`'s.
class AccessTable final: public DBTable<ACC_ID, const access_t> {
    public:
        virtual const std::pair<AccessTable::iterator, bool> insert(const access_t& entry) override final
        {
            InsAccessMap_[entry.instruction_id].push_back(entry.id); // create 1:n associations 
            return Map_.insert(typename std::map<ACC_ID, const access_t>::value_type(entry.id, entry));
        }

        virtual AccessTable::iterator insert(AccessTable::iterator hint, const access_t& entry) override final
        {
            InsAccessMap_[entry.instruction_id].push_back(entry.id); // create 1:n associations 
            return Map_.insert(hint, typename std::map<ACC_ID, const access_t>::value_type(entry.id, entry));
        }

        typedef std::vector<ACC_ID> accessVector_t;
        typedef std::map<INS_ID, accessVector_t> insAccessMap_t;

        /// Returns the internal map from the _instruction ID_ to the _access IDs_.
        const insAccessMap_t& getInsAccessMap() const noexcept
        { return InsAccessMap_; }; 

    private:
        /// Maps an intruction ID _I_ to the vectors of access IDs happening at the instruction _I_.
        insAccessMap_t InsAccessMap_;
};

#endif
