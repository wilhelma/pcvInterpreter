/**
 *
 *    @file  ShadowCall.h
 *   @brief  
 *
 *    @date  12/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SHADOW_CALL_H_
#define  SHADOW_CALL_H_

#include "Function.h" // <- for FunctionType
#include "Types.h"

/// @ingroup shadows
/// @brief Shadow abstraction for a function call.
class ShadowCall {
public:
    /// @brief Constructor.
    explicit ShadowCall(const TIME& call_time,
                        const TIME& runtime,           
                        const FUN_ID& function_id,
                        FunctionType function_type,
                        const SEG_ID& segment_id) noexcept    
        : CallTime_(call_time),
          Runtime_(runtime),
          FunctionId_(function_id),
          FunctionType_(function_type),
          SegmentId_(segment_id)
        {}
    
    /// _Default_ destructor.
    ~ShadowCall() = default;

    /// _Deleted_ copy constructor.
    ShadowCall(const ShadowCall&) = delete;
    /// _Deleted_ move constructor.
    ShadowCall(ShadowCall&&)      = delete;
    /// _Deleted_ copy assignment operator.
    ShadowCall& operator=(const ShadowCall&) = delete;
    /// _Deleted_ move assignment operator.
    ShadowCall& operator=(ShadowCall&&)      = delete;

    /// Returns the time when the function was called.
    const TIME& callTime() const noexcept
    { return CallTime_; }

    /// Returns the runtime of the function.
    const TIME& runtime() const noexcept
    { return Runtime_; }

    /// Returns the ID of the called function.
    const FUN_ID& functionId() const noexcept
    { return FunctionId_; }

    /// Returns the function type.
    const FunctionType functionType() const noexcept
    { return FunctionType_; }

    /// Returns the segment ID of the call.
    const SEG_ID& segmentId() const noexcept
    { return SegmentId_; }

private:
    /// The time when the function was called.
    TIME CallTime_;
    /// The runtime of the function.
    TIME Runtime_;
    /// The ID of the called function.
    FUN_ID FunctionId_;
    /// The type of the called function.
    FunctionType FunctionType_;
    /// The call segment ID.
    SEG_ID SegmentId_;
};

#endif
