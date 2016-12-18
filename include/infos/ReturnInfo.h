/**
 *
 *    @file  ReturnInfo.h
 *   @brief 
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RETURN_INFO_H_
#define  RETURN_INFO_H_

#include "fwd/ShadowCallMap.h"

#include "FunctionInfo.h"
#include "Types.h"

/// @ingroup info
/// @brief Information about a function return.
class ReturnInfo : public FunctionInfo {
public:
    /// @brief Constructor.
    /// @param call_it  Information about the function call.
    /// @param end_time The time when the function has returned.
    explicit ReturnInfo(ShadowCallMap::const_iterator call_it,
                        const TIME& end_time) noexcept
        : FunctionInfo(call_it),
          EndTime_(end_time)
        {}

    /// Returns the time when the function has returned.
    const TIME& endTime() const noexcept
    { return EndTime_; }

private:
    /// The time when the function has returned.
    TIME EndTime_;
};

#endif
