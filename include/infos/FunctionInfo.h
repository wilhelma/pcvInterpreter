/**
 *
 *    @file  FunctionInfo.h
 *   @brief  
 *
 *    @date  12/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FUNCTION_INFO_H_
#define  FUNCTION_INFO_H_

#include "fwd/Function.h"

#include "ShadowCallMap.h"
#include "Types.h"

/// @ingroup info
/// @brief Common information for calls and returns.
class FunctionInfo {
public:
    /// @brief Constructor.
    /// @param call_it Information about the call.
    explicit FunctionInfo(ShadowCallMap::const_iterator call_it) noexcept
        : Call_(call_it)
        {}

    /// Return the call ID of the function.
    const CAL_ID& callId() const noexcept;

    /// Returns the time when the function was called.
    const TIME& callTime() const noexcept;

    /// Returns the runtime of the function.
    const TIME& runtime() const noexcept;

    /// Returns the function type.
    const FunctionType functionType() const noexcept;

    /// The ID of the called function.
    const FUN_ID& functionId() const noexcept;

    /// Returns the segment ID of the call.
    const SEG_ID& segmentId() const noexcept;

private:
    /// Shadow information about the call.
    ShadowCallMap::const_iterator Call_;
};

#endif
