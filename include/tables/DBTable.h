/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <vector>

/// @defgroup tables
/// @ingroup database
/// @brief Classes in this group mimic a database table.
/// @details The idea is that each class is a wrapper around a map
/// between an ID type and a database record, e.g. `std::map<ACC_ID, access_t>`.


/// @ingroup tables
/// @brief Base class (default implementation) for all the tables.
/// @todo The ID in the record is redundant. Consider removing it and use the vector index, instead.
template<typename IdT, typename T>
class DBTable {
public:
    /// @brief The size type of the table.
    using size_type  = typename std::vector<T>::size_type;
    /// @brief The database row type.
    using value_type = T;
    /// @brief The database ID.
    using index_type = IdT;
    /// @brief The iterator on the table entries.
    using iterator       = typename std::vector<T>::iterator;
    /// @brief The constant iterator on the table entries.
    using const_iterator = typename std::vector<T>::const_iterator;

    /// _Default_ constructor.
    constexpr explicit DBTable() = default;

    /// _Default_ destructor.
    virtual ~DBTable() = default;

    /// _Deleted_ copy constructor.
    DBTable(const DBTable&) = delete;
    /// _Deleted_ move constructor.
    DBTable(DBTable&&)      = delete;
    /// _Deleted_ copy assignment operator.
    DBTable& operator=(const DBTable&) = delete;
    /// _Deleted_ move assignment operator.
    DBTable& operator=(DBTable&&)      = delete;

    /// @brief Inserts the _entry_ in the position before the one pointed to by _hint_.
    /// @param hint  Iterator pointing to the position where to insert.
    /// @param entry The entry to insert.
    virtual iterator insert(iterator hint, value_type&& entry)
    { return Vector_.emplace(hint, std::forward<value_type>(entry)); }
    
    /// @brief Reserve memory for the internal vector.
    /// @param new_cap The number of elements to reserve.
    void reserve(size_type new_cap)
    { Vector_.reserve(new_cap); }

    /// @brief Returns the element with database ID _pos_ (with boundary checking).
    /// @param pos The database ID to query for.
    /// @throws std::out_of_range If _pos_ is not smaller than _size()_.
    const value_type& at(const index_type& pos) const
    {
        auto tmp = idToVectorIndex(pos);
        return Vector_.at(tmp);
    }

    /// Size of the internal map.
    const std::size_t size() const noexcept
    { return Vector_.size(); };

    /// Begin iterator.
    iterator begin() noexcept
    { return Vector_.begin(); };

    /// End iterator.
    iterator end() noexcept
    { return Vector_.end(); };

    /// Begin const_iterator.
    const_iterator begin() const noexcept
    { return Vector_.begin(); };

    /// End const_iterator.
    const_iterator end() const noexcept
    { return Vector_.end(); };

    /// Begin const_iterator.
    const_iterator cbegin() const noexcept
    { return Vector_.cbegin(); };
    
    /// End const_iterator.
    const_iterator cend() const noexcept
    { return Vector_.cend(); };

protected:                                  
    /// The vector of database rows.
    std::vector<T> Vector_;

    /// @brief Helper function to convert database ID's to vector indices.
    /// @attention ID's start at 1, so far!
    virtual const size_type idToVectorIndex(const index_type& id) const
    {
        size_type tmp = static_cast<size_type>(id);
        return tmp - 1;
    }
};

/// @brief Helper function to allow `std::copy`ing on the table.
/// @param table Pointer to the `DBtable` to operate upon.
/// @tparam T The table type (must inherit from DBTable).
/// @tparam enable Will be empty if _T_ doesn't inherit from _DBTable_, thus aborting the compilation.
template <typename T,
          typename enable = std::enable_if_t<std::is_base_of<DBTable<typename T::index_type, typename T::value_type>, T>::value>>
std::insert_iterator<T> inserter(T* table) noexcept
{ return std::insert_iterator<T>(*table, table->end()); }

#endif
