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

#include <iterator>
#include <map>
#include <memory>

// -- Needed for template instantiation --
#include "ShadowLock.h"
#include "ShadowThread.h"
#include "Types.h"
// ---------------------------------------

template <typename IdT, typename T>
const std::shared_ptr<const T> ShadowMap<IdT, T>::getShadow(const IdT id) noexcept {
    // Get the Shadow corresponding to id
    const auto& shadow_it = Map_.find(id);
    if (shadow_it != std::cend(Map_))
        return shadow_it->second;

    // If the entry is not in the map, make a new one
    std::shared_ptr<const T> shadow = std::make_unique<const T>(id);
    Map_.insert(std::make_pair(id, shadow)); 

    return shadow;
}

// Instantiation for the ShadowThreadMap
template class ShadowMap<TRD_ID, ShadowThread>;
// Instantiation for the ShadowLockMap
template class ShadowMap<REF_ID, ShadowLock>;
