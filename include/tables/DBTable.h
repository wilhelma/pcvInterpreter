/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <map>

/// Maps IDs to structs wrapping database table entries.
template<typename IdT, typename T>
class DBTable {
public:
	/// @typedef DBTable::value_type
	/// Convenience definition to use `std::inserter`
	using value_type     = T;

	/// @typedef DBTable::iterator
	/// Convenience definition to use `std::inserter`
	using iterator       = typename std::map<IdT, T>::iterator;

	/// @typedef DBTable::const_iterator
	/// @todo Is this needed?
	using const_iterator = typename std::map<IdT, T>::const_iterator;

	/// Default constructor.
	DBTable() {};

	/// Default destructor.
	virtual ~DBTable() {};

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
	/// @todo Needed?
	const_iterator begin() const noexcept
	{ return Map_.begin(); };

	/// End const_iterator.
	/// @todo Needed?
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

	// prevent generated functions
	DBTable(const DBTable&);
	DBTable& operator=(const DBTable&);		 
};


#endif /* DBTABLE_H_ */
