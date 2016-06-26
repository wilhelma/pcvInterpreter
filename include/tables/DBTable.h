/*
* DBDataModel.h
*
*  Created on: Sep 12, 2014
*      Author: wilhelma
*/

#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <map>
#include <sqlite3.h>

template<typename IdT, typename T>
class DBTable
{
public:
	typedef std::map<IdT, T> Map;
	typedef typename Map::iterator iterator;
	typedef typename Map::const_iterator const_iterator;

	DBTable();
	~DBTable();

	int get(const IdT& id, T* entry);
	int insert(const IdT& id, const T& entry);
	virtual int fill(sqlite3_stmt *sqlstmt) = 0;
	
	iterator find(const IdT& id);
	const_iterator find(const IdT& id) const;
	unsigned size() const;

	iterator		begin();
	const_iterator	begin() const;
	iterator		end();
	const_iterator	end() const;

private:								  	
	Map map_;
	
	// prevent generated functions
	DBTable(const DBTable&);
	DBTable& operator=(const DBTable&);		 
};

#include "DBTable-inl.h"

#endif /* DBTABLE_H_ */
