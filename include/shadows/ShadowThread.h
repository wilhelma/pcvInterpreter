/*
 * ShadowThread.h
 *
 *  Created on: Sep 1, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_THREAD_H_
#define SHADOW_THREAD_H_

#include "Types.h"

/// @todo Document this!
/// @todo Complete implementation: it doesn't make sense to
/// define comparison operators if threadId is publicly accessible!
struct ShadowThread {
	const TRD_ID threadId;
//	CALLSITE currentCallSiteID;
//	FUN_SG currentFunctionSignature; 

	/// Constructor.
	explicit ShadowThread(TRD_ID threadId) noexcept
		: threadId(threadId)
	{};
	/// _Default_ destructor.
	~ShadowThread() = default;

	/// _Deleted_ copy constructor.
	ShadowThread(const ShadowThread&)            = delete;
	/// _Deleted_ move constructor.
	ShadowThread(ShadowThread&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	ShadowThread& operator=(const ShadowThread&) = delete;
	/// _Deleted_ move assignment operator.
	ShadowThread& operator=(ShadowThread&&)      = delete;

	/// @brief Less-than operator.
	/// @details Compares the `ThreadId_`s.
	friend const bool operator<(const ShadowThread& lhs, const ShadowThread& rhs)
	{ return lhs.threadId < rhs.threadId; };

	/// @brief Equality operator.
	/// @details Compares the `ThreadId_`s.
	friend const bool operator==(const ShadowThread& lhs, const ShadowThread& rhs)
	{ return lhs.threadId == rhs.threadId; };
};

/// @brief Equal-or-greater-than operator.
/// @return Negation of operator `ShadowThread::operator<()`.
inline const bool operator>=(const ShadowThread& lhs, const ShadowThread& rhs)
{ return !(lhs < rhs); }

#endif /* SHADOWTHREAD_H_ */
