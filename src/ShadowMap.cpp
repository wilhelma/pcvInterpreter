/**
 *
 *    @file  ShadowMap.cpp
 *   @brief  
 *
 *    @date  11/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ShadowMap.h"
#include "ShadowMapException.h"

#include <iterator>
#include <map>
#include <memory>

// -- Needed for template instantiation --
#include "ShadowLock.h"
#include "ShadowThread.h"
#include "ShadowVariable.h"
#include "Types.h"
// ---------------------------------------

//template <typename IdT, typename Shadow>
//const std::shared_ptr<const Shadow> ShadowMap<IdT, Shadow>::getShadow(const IdT& id) noexcept {
//    // Get the Shadow corresponding to id
//    const auto& shadow_it = Map_.find(id);
//    if (shadow_it != std::cend(Map_))
//        return shadow_it->second;
//
//    // If the entry is not in the map, make a new one
//    const std::shared_ptr<const Shadow>& shadow = new_entry(id);
//    Map_.insert(std::make_pair(id, shadow)); 
//
//    return shadow;
//}

template <typename IdT, typename Shadow>
typename ShadowMap<IdT, Shadow>::iterator ShadowMap<IdT, Shadow>::erase(const IdT& id) {
    const auto& shadow_it = Map_.find(id);

    // In this case, the interpreter is probably buggy
    if (shadow_it == std::cend(Map_))
        throw ShadowMapException("Entry " + std::to_string(id) + " not found",
                                 "ShadowMap::remove");

    // In this case, the pointer is still in use somewhere
    if (!shadow_it->second.unique())
        throw ShadowMapException("Pointer to Shadow " + std::to_string(id) + " still in use",
                                 "ShadowMap::remove");

    return Map_.erase(shadow_it);
}

// Instantiation for the ShadowThreadMap
template class ShadowMap<TRD_ID, ShadowThread>;
// Instantiation for the ShadowLockMap
template class ShadowMap<REF_ID, ShadowLock>;
// Instantiation for the ShadowVariableMap
template class ShadowMap<REF_ID, ShadowVariable>;
