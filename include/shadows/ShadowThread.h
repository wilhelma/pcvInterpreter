/*
 * ShadowThread.h
 *
 *  Created on: Sep 1, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_THREAD_H_
#define SHADOW_THREAD_H_

#include "Types.h"

/// @ingroup shadows
/// @brief Shadow information about a thread.
class ShadowThread {
public:
	/// @brief Constructor.
    /// @param start_time The time when the thread was created.
	constexpr explicit ShadowThread(const TIME& start_time) noexcept
		: StartTime_(start_time)
	{}

	/// _Default_ destructor.
	~ShadowThread() = default;

	/// _Deleted_ copy constructor.
	ShadowThread(const ShadowThread&) = delete;
	/// _Deleted_ move constructor.
	ShadowThread(ShadowThread&&)      = delete;
	/// _Deleted_ copy assignment operator.
	ShadowThread& operator=(const ShadowThread&) = delete;
	/// _Deleted_ move assignment operator.
	ShadowThread& operator=(ShadowThread&&)      = delete;

    /// Returns the time when the thread was created.
    constexpr const TIME& startTime() const noexcept
    { return StartTime_; }

private:
    /// The time when the thread has been created.
    TIME StartTime_;
};

#endif
