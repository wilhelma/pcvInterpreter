
#include "Interpreter.h"
#include <boost/log/trivial.hpp>

template<typename IdT, typename T>
const int DBTable<IdT, T>::get(const IdT& id, T* entry) const {
	auto search = find(id);
	if (search != cend()) {
		entry = &search->second;
		return IN_OK;
	} else {
		BOOST_LOG_TRIVIAL(error) << typeid(T).name()
								 << " not found: " << id;
		entry = nullptr;
		return IN_NO_ENTRY;
	}
}
