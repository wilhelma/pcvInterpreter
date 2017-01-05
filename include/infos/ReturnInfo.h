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

/// @ingroup info
/// @brief Information about a function return.
class ReturnInfo : public FunctionInfo {
public:
    /// @brief Constructor.
    /// @param call_it  Information about the function call.
    explicit ReturnInfo(ShadowCallMap::const_iterator call_it) noexcept
        : FunctionInfo(call_it)
        {}
};

#endif
