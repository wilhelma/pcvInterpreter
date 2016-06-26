
#include "Interpreter.h"
#include <boost/log/trivial.hpp>

template<typename IdT, typename T>
DBTable<IdT, T>::DBTable() {}

template<typename IdT, typename T>
DBTable<IdT, T>::~DBTable() { }

template<typename IdT, typename T>
int DBTable<IdT, T>::insert(const IdT& id, const T& entry) {
	if ( map_.find( id ) != map_.end() )
		return IN_ENTRY_EXISTS;

	map_.insert( typename std::map<IdT, T>::value_type(id, entry) );
	return IN_OK;
}

template<typename IdT, typename T>
int DBTable<IdT, T>::get(const IdT& id, T* entry) {

	auto search = map_.find(id);
	if (search != map_.end()) {
		entry = &search->second;
		return IN_OK;
	} else {
		BOOST_LOG_TRIVIAL(error) << typeid(T).name()
								 << " not found: " << id;
		entry = nullptr;
		return IN_NO_ENTRY;
	}
}

template<typename IdT, typename T>
unsigned DBTable<IdT, T>::size() const {
	return map_.size();
}

template<typename IdT, typename T>
typename DBTable<IdT, T>::iterator DBTable<IdT, T>::find(const IdT& id) {
	return map_.find(id);
}

template<typename IdT, typename T>
typename DBTable<IdT, T>::const_iterator DBTable<IdT, T>::find(const IdT& id) const {
	return map_.find(id);
}

template<typename IdT, typename T>
typename DBTable<IdT, T>::iterator DBTable<IdT, T>::begin() {
	return map_.begin(); 
}

template<typename IdT, typename T>
typename DBTable<IdT, T>::const_iterator DBTable<IdT, T>::begin() const {
	return map_.begin(); 
}

template<typename IdT, typename T>
typename DBTable<IdT, T>::iterator DBTable<IdT, T>::end() { 
	return map_.end(); 
}

template<typename IdT, typename T>
typename DBTable<IdT, T>::const_iterator DBTable<IdT, T>::end() const { 
	return map_.end(); 
}
