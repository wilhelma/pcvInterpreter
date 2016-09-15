/*
 * ThreadMgr.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef THREAD_MGR_H_
#define THREAD_MGR_H_

#include <map>
#include "ShadowThread.h"
#include "Types.h"

/// @todo Document this!
class ThreadMgr {
public:
	/// _Default_ constructor.
  explicit ThreadMgr() = default;
	/// _Default_ destructor.
	~ThreadMgr()                   = default;

	/// _Deleted_ copy constructor.
	ThreadMgr(const ThreadMgr&)            = delete;
	/// _Deleted_ move constructor.
	ThreadMgr(ThreadMgr&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	ThreadMgr& operator=(const ThreadMgr&) = delete;
	/// _Deleted_ move assignment operator.
	ThreadMgr& operator=(ThreadMgr&&)      = delete;

	/// @brief Get a pointer to the `ShadowThread` associated with `threadId`.
	/// @details Looks for `threadId` in the map. If it's found, returns
	/// the `ShadowThread` associated with it; otherwise makes a new entry
	/// in the maps and increments `currentThreadId_`.
	const ShadowThread* const getThread(const TRD_ID& threadId);

	/// @brief `delete`s the `ShadowThread` associated with the `threadId`
	/// and deletes the entry from the map.
	void threadJoined(const TRD_ID& threadId);

private:
	/// @todo Document this!
	static TRD_ID currentThreadId_;

	/// @typedef TIdThreadMap.
	/// A map from the thread ID to the corresponding
	/// `ShadowThread` pointer.
	using TIdThreadMap_ = std::map<TRD_ID, const ShadowThread* const>;
	TIdThreadMap_ tIdThreadMap_;
};


#endif /* THREADMGR_H_ */
