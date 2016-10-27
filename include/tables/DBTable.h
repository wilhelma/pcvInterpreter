/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <map>
#include <memory>

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

    /// @typedef DBTable::value_type
    /// @brief Convenience definition to optionally activate the inserter.
    using key_type       = IdT;

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

/// @brief Helper function to allow `std::copy` on the table.
/// @param table Pointer to the `DBtable` to operate upon.
/// @tparam T The table type (must inherit from DBTable).
/// @attention The second parameter argument will only check that
/// T actually inherits from DBTable. If not, a compile-time error
/// will be issued.
template <typename T,
          typename = std::enable_if_t<std::is_base_of<DBTable<typename T::key_type, typename T::value_type>, T>::value>>
std::insert_iterator<T> inserter(T* table) noexcept
{ return std::insert_iterator<T>(*table, table->end()); }

#endif /* DBTABLE_H_ */
