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

#include "fwd/Function.h"

#include "Types.h"

#include <cassert>

/// @ingroup shadows
/// @brief Shadow abstraction for a function call.
class ShadowCall {
public:
    /// @brief Constructor.
    /// @param call_time     The time when the function was called.
    /// @param return_time   The time when the function returned.
    /// @param function_id   The ID of the function.
    /// @param function_type The type of the function.
    /// @param segment_id    The segment ID of the function.
    constexpr explicit ShadowCall(const TIME& call_time,
                                  const TIME& return_time,           
                                  const FUN_ID& function_id,
                                  FunctionType function_type,
                                  const SEG_ID& segment_id) noexcept    
        : CallTime_(call_time),
          Runtime_(return_time - call_time),
          ReturnTime_(return_time),
          FunctionId_(function_id),
          FunctionType_(function_type),
          SegmentId_(segment_id)
        { assert(return_time >= call_time); }
    
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
    constexpr const TIME& callTime() const noexcept
    { return CallTime_; }

    /// Returns the runtime of the function.
    constexpr const TIME& runtime() const noexcept
    { return Runtime_; }

    /// Returns the time when the function returned.
    constexpr const TIME& returnTime() const noexcept
    { return ReturnTime_; }

    /// Returns the ID of the called function.
    constexpr const FUN_ID& functionId() const noexcept
    { return FunctionId_; }

    /// Returns the function type.
    constexpr const FunctionType functionType() const noexcept
    { return FunctionType_; }

    /// Returns the segment ID of the call.
    constexpr const SEG_ID& segmentId() const noexcept
    { return SegmentId_; }

private:
    /// The time when the function was called.
    TIME CallTime_;
    /// The runtime of the function.
    TIME Runtime_;
    /// The return time of the function.
    TIME ReturnTime_;
    /// The ID of the called function.
    FUN_ID FunctionId_;
    /// The type of the called function.
    FunctionType FunctionType_;
    /// The call segment ID.
    SEG_ID SegmentId_;
};

#endif
