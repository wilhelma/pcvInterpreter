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
struct CallInfo : public FunctionInfo {
    /// Constructor.
    explicit CallInfo(ShadowCallMap::const_iterator call_it,
                      const CALLSITE& site_id,
                      const FUN_SG& function_signature) noexcept
        : FunctionInfo(call_it),
          SiteId_(site_id),
          FunctionSignature_(function_signature)
        {}

    const CALLSITE& siteId() const noexcept
    { return SiteId_; }

    const FUN_SG& functionSignature() const noexcept
    { return FunctionSignature_; }

private:
    CALLSITE SiteId_;
    FUN_SG   FunctionSignature_;
};

#endif
