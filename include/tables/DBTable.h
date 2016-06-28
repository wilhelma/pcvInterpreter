/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <map>
#include <utility>

#include <sqlite3.h>

template<typename IdT, typename T>
class DBTable
{
public:
	typedef std::map<IdT, T> Map;
	typedef typename Map::iterator iterator;
	typedef typename Map::const_iterator const_iterator;

	DBTable() {};
	virtual ~DBTable() {};

	const int get(const IdT& id, T* entry) const;
	virtual const std::pair<iterator, bool> fill(sqlite3_stmt *sqlstmt) = 0;
	
	iterator find(const IdT& id)
	{ return map_.find(id); };
	
	const_iterator find(const IdT& id) const
	{ return map_.find(id); };
	
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

private:								  	
	Map map_;

	// prevent generated functions
	DBTable(const DBTable&);
	DBTable& operator=(const DBTable&);		 

protected:
	std::pair<iterator, bool> insert(const IdT& id, const T& entry)
	{ return map_.insert(typename std::map<IdT, T>::value_type(id, entry)); };
};

#include "DBTable-inl.h"

#endif /* DBTABLE_H_ */
