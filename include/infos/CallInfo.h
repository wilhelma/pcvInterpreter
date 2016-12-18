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
#include "ShadowCall.h"

#include "Function.h" // <- for FunctionType
#include "Types.h"

/// @ingroup info
/// @brief Information about a function call.
struct CallInfo {
    /// Constructor.
    explicit CallInfo(ShadowCallMap::const_iterator call_it,
                      const CALLSITE& site_id,
                      const FUN_SG& function_signature) noexcept
        : Call_(call_it),
          SiteId_(site_id),
          FunctionSignature_(function_signature)
        {}

    /// Returns the time when the function was called.
    const TIME& callTime() const noexcept
    { return Call_->second->callTime(); }

    /// Returns the runtime of the function.
    const TIME& runtime() const noexcept
    { return Call_->second->runtime(); }

    /// Returns the function type.
    const FunctionType functionType() const noexcept
    { return Call_->second->functionType(); }

    /// Returns the segment ID of the call.
    const SEG_ID& segmentId() const noexcept
    { return Call_->second->segmentId(); }

    const CALLSITE& siteId() const noexcept
    { return SiteId_; }

    const FUN_SG& functionSignature() const noexcept
    { return FunctionSignature_; }

private:
    /// Shadow information about the call.
    ShadowCallMap::const_iterator Call_;

    CALLSITE SiteId_;
    FUN_SG   FunctionSignature_;
};

#endif
