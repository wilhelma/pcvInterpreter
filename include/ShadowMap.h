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

#include <map>
#include <memory>

/// @brief Maps `Shadow` abstractions to their ID.
template<typename IdT, typename T>
class ShadowMap {
public:
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

    /// @brief Get a pointer to the `Shadow` associated with the ID.
    /// @details If no entry esists with the queried ID, create a new one 
    /// and return that one.
    /// @param id The ID of the `Shadow` to query for.
    const std::shared_ptr<const T> getShadow(const IdT id) noexcept;

private:
    /// Map the ID to the corresponding `Shadow` pointer.
    std::map<const IdT, const std::shared_ptr<const T>> Map_;
};

#endif
