/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <map>

/// @defgroup tables
/// @ingroup database
/// @brief Classes in this group mimic a database table.
/// @details The idea is that each class is a wrapper around a map
/// between an ID type and a database record, e.g. `std::map<ACC_ID, access_t>`.


/// @ingroup tables
/// @brief Base class (default implementation) for all the tables.
/// @todo The ID in the record is redundant. Consider removing it.
template<typename IdT, typename T>
class DBTable {
public:
    /// @typedef DBTable::value_type
    /// @brief Convenience definition to use `std::inserter`.
    using value_type     = T;

    /// @typedef DBTable::iterator
    /// @brief Convenience definition to use `std::inserter`.
    using iterator       = typename std::map<IdT, T>::iterator;

    /// @typedef DBTable::const_iterator
    /// @brief Convenience definition to use `std::inserter`.
    /// @todo Is this needed?
    using const_iterator = typename std::map<IdT, T>::const_iterator;

    /// _Default_ constructor.
    constexpr explicit DBTable() = default;

    /// _Default_ destructor.
    virtual ~DBTable() = default;

    /// _Deleted_ copy constructor.
    DBTable(const DBTable&)            = delete;
    /// _Deleted_ move constructor.
    DBTable(DBTable&&)                 = delete;
    /// _Deleted_ copy assignment operator.
    DBTable& operator=(const DBTable&) = delete;
    /// _Deleted_ move assignment operator.
    DBTable& operator=(DBTable&&)      = delete;

    /// Inserts the entry in the maps creating a `pair(id, entry)`.
    virtual const std::pair<iterator, bool> insert(const T& entry)
    { return Map_.insert(typename std::map<IdT, T>::value_type(entry.id, entry)); }

    /// @brief Inserts the `entry` in the position pointed to by `hint`.
    /// @param hint  Iterator pointing to the position where to insert `entry`.
    /// @param entry The entry to insert.
    /// @attention This is a convenience function only needed to allow
    /// `std::copy` to be used on `DBTable`.
    virtual DBTable::iterator insert(DBTable::iterator hint, const T& entry)
    { return Map_.insert(hint, typename std::map<IdT, T>::value_type(entry.id, entry)); }

    /// Inserter to allow `std::copy`.
    /// @param table The `DBtable` to operate upon.
    friend std::insert_iterator<DBTable> inserter(DBTable& table)
    { return std::insert_iterator<DBTable>(table, table.end()); }
    
    /// Find `id` in the map.
    iterator find(const IdT& id)
    { return Map_.find(id); };
    
    /// Find `id` in the map.
    const_iterator find(const IdT& id) const
    { return Map_.find(id); };
    
    /// Size of the internal map.
    const std::size_t size() const noexcept
    { return Map_.size(); };

    /// Begin iterator.
    iterator begin() noexcept
    { return Map_.begin(); };

    /// End iterator.
    iterator end() noexcept
    { return Map_.end(); };

    /// Begin const_iterator.
    const_iterator begin() const noexcept
    { return Map_.begin(); };

    /// End const_iterator.
    const_iterator end() const noexcept
    { return Map_.end(); };

    /// Begin const_iterator.
    const_iterator cbegin() const noexcept
    { return Map_.cbegin(); };
    
    /// End const_iterator.
    const_iterator cend() const noexcept
    { return Map_.cend(); };

protected:                                  
    /// The map associating IDs to database table entries.
    std::map<IdT, T> Map_;
};


#endif /* DBTABLE_H_ */
