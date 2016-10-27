/*
 * ThreadMgr.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef THREAD_MGR_H_
#define THREAD_MGR_H_

#include "fwd/ShadowThread.h"

#include "Types.h"

#include <map>
#include <memory>

/// @brief Maps a thread ID to the corresponding ShadowThread.
class ThreadMgr {
public:
	/// Constructor.
    explicit ThreadMgr() noexcept
        : CurrentThreadId_(0)
    {};

	/// _Default_ destructor.
	~ThreadMgr()         = default;

	/// _Deleted_ copy constructor.
	ThreadMgr(const ThreadMgr&)            = delete;
	/// _Deleted_ move constructor.
	ThreadMgr(ThreadMgr&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	ThreadMgr& operator=(const ThreadMgr&) = delete;
	/// _Deleted_ move assignment operator.
	ThreadMgr& operator=(ThreadMgr&&)      = delete;


	/// @brief Get a pointer to the `ShadowThread` associated with `threadId`.
    /// @details If no entry with the queried thread ID exist, create a new
    /// one and return that one.
    /// @param thread_id The thread ID to query for.
	const std::shared_ptr<const ShadowThread> getThread(const TRD_ID& threadId) noexcept;

	/// @brief `delete`s the `ShadowThread` associated with the `threadId`
	/// and deletes the entry from the map.
    /// @throw Whatever The queried thread ID is not in the map.
    /// @throw Whatever The pointer corresponding to the thread ID hasn't expired.
	void threadJoined(const TRD_ID& threadId);

private:
    /// The current thread ID.
	TRD_ID CurrentThreadId_;

	/// Map the thread ID to the corresponding `ShadowThread` pointer.
    std::map<const TRD_ID, const std::shared_ptr<const ShadowThread>> ThreadIdToShadowThread_;
};

#endif
