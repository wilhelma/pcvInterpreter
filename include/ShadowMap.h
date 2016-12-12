/**
 *
 *    @file  ShadowMap.h
 *   @brief  
 *
 *    @date  11/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SHADOW_MAP_H_
#define  SHADOW_MAP_H_

#include "fwd/Database.h"

#include <map>
#include <memory>
#include <utility>
#include <type_traits>

/// @ingroup shadows
/// @brief Maps shadoss abstractions to their ID.
/// @tparam IdT    ID of the type one wants to associate to the shadow.
/// @tparam Shadow The shadow type.
template<typename IdT, typename Shadow>
class ShadowMap {
public:
    /// The shadow type.
    using shadow_type    = Shadow;
    /// Key type of the ShadowMap.
    using key_type       = IdT;
    /// Mapped type of the ShadowMap.
    using mapped_type    = const std::shared_ptr<const Shadow>;
    /// Value type of the ShadowMap.
    using value_type     = std::pair<const key_type, mapped_type>;
    /// Iterator on the ShadowMap.
    using iterator       = typename std::map<const key_type, mapped_type>::iterator;
    /// Constant iterator on the ShadowMap.
    using const_iterator = typename std::map<const key_type, mapped_type>::const_iterator;

    /// _Default_ constructor.
    explicit ShadowMap() = default;
    /// _Default_ destructor.
    virtual ~ShadowMap() = default;

    /// _Deleted_ copy constructor.
    ShadowMap(const ShadowMap&) = delete;
    /// _Deleted_ move constructor.
    ShadowMap(ShadowMap&&)      = delete;

    /// _Deleted_ copy assignment operator.
    ShadowMap& operator=(const ShadowMap&) = delete;
    /// _Deleted_ move assignment operator.
    ShadowMap& operator=(ShadowMap&&)      = delete;

    /// @brief Erase an entry from the map.
    /// @param id The key id of the element to remove.
    /// @return Iterator following the removed element.
    /// @throw ShadowMapException If the queried ID is not in the map.
    /// @throw ShadowMapException If the pointer corresponding to the ID is still in use.
    /// @todo Probably change the exception specification to one resembling the STL.
    iterator erase(const IdT& id);

    /// @brief Erase an entry from the map.
    /// @param it The iterator to the element to erase.
    iterator erase(const_iterator it) noexcept
    { return Map_.erase(it); }

    /// Insert an element.
    virtual const std::pair<iterator, bool> insert(const value_type& value) noexcept
    { return Map_.insert(value); }

    /// Find element in the map.
    /// @param id The key of the element to look for.
    const_iterator find(const IdT& id) const noexcept
    { return Map_.find(id); }

    /// Return a constant iterator to the element after the last one.
    const_iterator cend() const noexcept
    { return Map_.cend(); }

    /// Return a constant iterator to the element after the last one.
    const_iterator end() const noexcept
    { return Map_.end(); }

    /// Return an iterator to the element after the last one.
    iterator end() noexcept
    { return Map_.end(); }

    /// Returns whether the map is empty.
    const bool empty() const noexcept
    { return Map_.empty(); }

private:
    /// Map the ID to the corresponding `Shadow` pointer.
    std::map<key_type, mapped_type> Map_;
};

/// @brief Get the `Shadow` pointer associated to the ID.
/// @details Look for _id_ in the map and, if it's found, return the associated `Shadow` pointer.
/// If the entry is not there, a new one will be created and returned.
/// @param id The key id to look for.
/// @param sm The `ShadowMap` to look in.
/// @param db Database reference to fetch information in if a new element has to be created.
/// @tparam SMap     The shadow-map type.
/// @tparam activate A template expression that will cause the compilation to fail if `SMap` 
///                  does not inherit from `ShadowMap`.
template <typename SMap,
          typename activate = std::enable_if_t<std::is_base_of<ShadowMap<typename SMap::key_type, typename SMap::shadow_type>, SMap>::value>>
typename SMap::mapped_type get_shadow(const typename SMap::key_type& id, SMap& sm, const Database& db) noexcept
{
    // Get the Shadow corresponding to id
    const auto& shadow_it = sm.find(id);
    if (shadow_it != std::cend(sm))
        return shadow_it->second;

    // If the entry is not in the map, make a new one
    typename SMap::mapped_type& shadow = sm.new_entry(id, db);
    sm.insert(std::make_pair(id, shadow)); 

    return shadow;
}

#endif
