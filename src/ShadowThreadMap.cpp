/**
 *
 *    @file  ShadowThreadMap.cpp
 *   @brief  
 *
 *    @date  11/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ShadowThreadMap.h"

#include "JoinInfo.h"
#include "NewThreadInfo.h"
#include "ShadowThread.h"
#include "Types.h"

#include <memory>

// Helper function.
// * Gets and iterator to the entry with key id in the map;
// * If the entry is not there, make it and return its iterator.
ShadowThreadMap::const_iterator iterator_of(const TRD_ID& id,
                                            const TIME& t,
                                            ShadowThreadMap& stm)
{
    const auto& thread_it = stm.find(id);

    // Create entry for the child, if it's not in the map
    if (thread_it == std::cend(stm)) {
        auto&& s_thread = std::make_shared<const ShadowThread>(t);
        return stm.insert(std::make_pair(id, std::move(s_thread))).first;
    }

    return thread_it;
}

std::unique_ptr<const NewThreadInfo> ShadowThreadMap::newThreadInfo(const TRD_ID& child_thread_id, const TIME& start_time, const TIME& run_time) noexcept
{
    const auto child_thread_it = iterator_of(child_thread_id, start_time, *this);
    return std::make_unique<const NewThreadInfo>(child_thread_it, run_time);
}

std::unique_ptr<const JoinInfo> ShadowThreadMap::joinInfo(const TRD_ID& child_thread_id, const TIME& join_time)
{
    const auto& child_thread_it = find(child_thread_id);

    // If the entry is not in the map, throw
    if (child_thread_it == cend())
        throw 1;

    return std::make_unique<const JoinInfo>(child_thread_it, join_time);
}
