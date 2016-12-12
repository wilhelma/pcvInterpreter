/**
 *
 *    @file  AcquireInfo.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACQUIRE_INFO_H_
#define  ACQUIRE_INFO_H_

#include "fwd/ShadowLockMap.h"

#include "LockInfo.h"
#include "Types.h"

#include <memory>

/// @ingroup info
/// @brief Information about the acquisition of a memory lock.
class AcquireInfo : public LockInfo {
public:
    /// @brief Constructor.
    /// @param lock_info Iterator to the ShadowLockMap entry where the lock information is stored.
    explicit AcquireInfo(ShadowLockMap::const_iterator lock_info) noexcept
        : LockInfo(lock_info)
    {}
};

#endif
