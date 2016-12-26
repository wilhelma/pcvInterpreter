/**
 *
 *    @file  CallInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_INFO_H_
#define  CALL_INFO_H_

#include "ShadowCallMap.h"

#include "FunctionInfo.h"
#include "Types.h"

/// @ingroup info
/// @brief Information about a function call.
class CallInfo : public FunctionInfo {
public:
    /// @brief Constructor.
    /// @param call_it            The information about the call.
    /// @param site_id            __What is this?__
    /// @param function_signature The signature of the function.
    explicit CallInfo(ShadowCallMap::const_iterator call_it,
                      const CALLSITE& site_id,
                      const FUN_SG& function_signature) noexcept
        : FunctionInfo(call_it),
          SiteId_(site_id),
          FunctionSignature_(function_signature)
        {}

    /// @todo Document this.
    const CALLSITE& siteId() const noexcept
    { return SiteId_; }

    /// Returns the signature of the called function.
    const FUN_SG& functionSignature() const noexcept
    { return FunctionSignature_; }

private:
    /// @todo Document this.
    CALLSITE SiteId_;
    /// @brief The signature of the called function.
    /// @attention So far this is only the function name.
    FUN_SG   FunctionSignature_;
};

#endif
