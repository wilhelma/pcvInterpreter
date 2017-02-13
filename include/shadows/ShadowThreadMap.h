/*
 * ShadowThreadMap.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_THREAD_MAP_H_
#define SHADOW_THREAD_MAP_H_

#include "ShadowMap.h"
#include "Types.h"

#include "fwd/Database.h"
#include "fwd/JoinInfo.h"
#include "fwd/NewThreadInfo.h"
#include "fwd/ShadowThread.h"

#include <memory>

/// @ingroup shadows
/// @brief Maps a thread ID to the corresponding ShadowThread.
class ShadowThreadMap final : public ShadowMap<TRD_ID, ShadowThread> {
public:

    /// @brief Creates a new information class about the creation of a thread.
    /// @param child_thread_id  The ID of the created thread.
    /// @param start_time       The time when the thread was created.
    /// @param run_time         The runtime of the thread.
    std::unique_ptr<const NewThreadInfo> newThreadInfo(const TRD_ID& child_thread_id, const TIME& start_time, const TIME& run_time) noexcept;

    /// @brief Creates a new information class about a thread join.
    /// @param child_thread_id The ID of the created thread.
    /// @param join_time       The time when the thread was joined.
    std::unique_ptr<const JoinInfo> joinInfo(const TRD_ID& child_thread_id, const TIME& join_time);
};

#endif
