/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include "fwd/SQLStatementIterator.h"

#include <map>
#include <utility>

#include <sqlite3.h>

template<typename IdT, typename T>
class DBTable {
public:
	/// @typedef TBTable::value_type
	/// Convenience definition to use `std::inserter`
	using value_type     = T;

	/// @typedef TBTable::iterator
	/// Convenience definition to use `std::inserter`
	using iterator       = typename std::map<IdT, T>::iterator;
	using const_iterator = typename std::map<IdT, T>::const_iterator;

	DBTable() {};
	virtual ~DBTable() {};

	virtual const std::pair<iterator, bool> insert(const T& entry)
	{ return map_.insert(typename std::map<IdT, T>::value_type(entry.id, entry)); }

	virtual DBTable::iterator insert(DBTable::iterator hint, const T& entry)
	{ return map_.insert(hint, typename std::map<IdT, T>::value_type(entry.id, entry)); }

	/// `DBTable`'s own inserter
	friend std::insert_iterator<DBTable> inserter(DBTable& table)
	{ return std::insert_iterator<DBTable>(table, table.end()); }
	
	iterator find(const IdT& id)
	{ return map_.find(id); };
	
	const_iterator find(const IdT& id) const
	{ return map_.find(id); };
	
	/// Size of the internal map
	const std::size_t size() const noexcept
	{ return map_.size(); };

	// iterators
	iterator begin() noexcept
	{ return map_.begin(); };

	iterator end() noexcept
	{ return map_.end(); };

	const_iterator begin() const noexcept
	{ return map_.begin(); };

	const_iterator end() const noexcept
	{ return map_.end(); };

	const_iterator cbegin() const noexcept
	{ return map_.cbegin(); };
	
	const_iterator cend() const noexcept
	{ return map_.cend(); };

protected:								  	
	std::map<IdT, T> map_;

	// prevent generated functions
	DBTable(const DBTable&);
	DBTable& operator=(const DBTable&);		 
};


#endif /* DBTABLE_H_ */
