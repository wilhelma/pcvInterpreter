/**
 *
 *    @file  Types.h
 *   @brief  Collects all the user-defined types.
 *
 *    @date  04/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <limits>

#include <type_traits>

template<bool is_true>
inline constexpr std::integral_constant<bool, is_true> summable() noexcept
{ return std::integral_constant<bool, is_true>{}; }

template<bool is_true>
inline constexpr std::integral_constant<bool, is_true> sortable() noexcept
{ return std::integral_constant<bool, is_true>{}; }

/// @defgroup types
/// @brief Types defined to handle the data read from the input database.


/// @ingroup types
/// @brief Struct to prevent implicit conversion among types.
/// @tparam T   The underlying type.
/// @tparam uid A _unique ID specifier_ to prevent implicit casts.
/// @todo When used with `T = std::string`, `string` methods cannot be called.
template <typename T, typename sum_enabled, size_t uid>
struct StrongTypedef {
    /// @brief Constructor.
    /// @param value The value to which initialize the data member.
    constexpr explicit StrongTypedef(T value) noexcept
        : Value_(value)
        {}

    /// _Default_ copy constructor.
    constexpr StrongTypedef(const StrongTypedef& input) noexcept = default;
    /// _Default_ move constructor.
    constexpr StrongTypedef(StrongTypedef&& input) noexcept      = default;

    /// _Default_ copy assignment operator.
    constexpr StrongTypedef& operator=(const StrongTypedef& input) noexcept = default;
    /// _Default_ move assignment operator.
    constexpr StrongTypedef& operator=(StrongTypedef&& input) noexcept      = default;

    /// Pre-increment operator.
//    template <typename U = StrongTypedef<T, sum_enabled, uid>,
//              typename = std::enable_if_t<std::is_same<sum_enabled, std::true_type>::value>>
    constexpr StrongTypedef& operator++() noexcept
    { Value_++; return *this; }

//    /// Pre-increment operator.
//    template <typename = std::enable_if_t<std::is_same<sum_enabled, std::false_type>::value>>
//    constexpr StrongTypedef& operator++() noexcept
//    { Value_++; return *this; }

    /// Post-increment operator.
//    template <typename U = StrongTypedef<T, sum_enabled, uid>,
//              typename = std::enable_if_t<std::is_same<sum_enabled, std::true_type>::value>>
    constexpr StrongTypedef operator++(int) noexcept
    { StrongTypedef old(*this); ++(*this); return old; }

//    /// Post-increment operator.
//    template <typename = std::enable_if_t<std::is_same<sum_enabled, std::false_type>::value>>
//    constexpr StrongTypedef operator++(int) noexcept
//    { StrongTypedef old(*this); ++(*this); return old; }

    /// @brief Implicit conversion operator (useful for streams).
    /// @return By value, if `T` is fundamental, by reference otherwise.
    constexpr operator
        std::conditional_t<std::is_fundamental<T>::value, const T, const T&> () const noexcept
    { return Value_; }

    /// @brief Equality operator.
    /// @param lhs The left-hand side.
    /// @param rhs The right-hand side.
    friend constexpr const bool operator==(const StrongTypedef& lhs, const StrongTypedef& rhs) noexcept
    { return lhs.Value_ == rhs.Value_; }

    /// @brief Less-than operator.
    /// @param lhs The left-hand side.
    /// @param rhs The right-hand side.
    friend constexpr const bool operator<(const StrongTypedef& lhs, const StrongTypedef& rhs) noexcept
    { return lhs.Value_ < rhs.Value_; }

    /// @brief Greater-than operator.
    /// @param lhs The left-hand side.
    /// @param rhs The right-hand side.
    friend constexpr const bool operator>(const StrongTypedef& lhs, const StrongTypedef& rhs) noexcept
    { return lhs.Value_ > rhs.Value_; }

private: 
    /// The value contained (i.e. hidden) in the struct.
    T Value_;
};

using is_summable = std::true_type;
using no_summable = std::false_type;


// All others IDs should have the same type as ID
using ID     = unsigned;
using ACC_ID = StrongTypedef<ID, no_summable,  0>;
using CAL_ID = StrongTypedef<ID, no_summable,  1>;
using FIL_ID = StrongTypedef<ID, no_summable,  2>;
using FUN_ID = StrongTypedef<ID, no_summable,  3>;
using INS_ID = StrongTypedef<ID, no_summable,  4>;
using LOP_ID = StrongTypedef<ID, no_summable,  5>;
using LOE_ID = StrongTypedef<ID, no_summable,  6>;
using LOI_ID = StrongTypedef<ID, no_summable,  7>;
using REF_ID = StrongTypedef<ID, no_summable,  8>;
using SEG_ID = StrongTypedef<ID, no_summable,  9>;
using TRD_ID = StrongTypedef<ID, no_summable, 10>;

static const ID NO_ID = std::numeric_limits<ID>::max();

// Access Types
using POS   = StrongTypedef<unsigned, no_summable, 11>;
// ACC_TYP Already uses AccessType
// MEM_ST  Already uses AccessState

// Call Types

using TIME = StrongTypedef<uint64_t, is_summable, 12>;

/// Increment operator for `TIME` variables.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
inline void operator+=(TIME& lhs, const TIME& rhs)
{ lhs = static_cast<TIME>(static_cast<uint64_t>(lhs) + rhs); }


/// Decrement operator for `TIME` variables.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
inline void operator-=(TIME& lhs, const TIME& rhs)
{ lhs = static_cast<TIME>(static_cast<uint64_t>(lhs) - rhs); }


/// Addition operator for `TIME` variables.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
inline constexpr TIME operator+(const TIME& lhs, const TIME& rhs) noexcept
{ return static_cast<TIME>(static_cast<uint64_t>(lhs) + rhs); }

/// Minus operator for `TIME` variables.
/// @param t The variable to take the value from.
/// @return A `TIME` variable whose value is the opposite
/// of `t`'s one.
inline constexpr TIME operator-(const TIME& t) noexcept
{ return static_cast<TIME>(-static_cast<uint64_t>(t)); }

/// Subtraction operator for `TIME` variables.
/// @param lhs The left-hand side.
/// @param lhs The right-hand side.
inline constexpr TIME operator-(const TIME& lhs, const TIME& rhs) noexcept
{ return lhs + (-rhs); }

using CALLSITE = StrongTypedef<size_t, no_summable,  13>;

// File Types
using FIL_PT = std::string;

// Function Types
using FUN_SG = std::string; 
// Function Name
using FUN_NM = std::string;
// FUN_TYP Already uses FunctionType
using LIN_NO = StrongTypedef<unsigned, no_summable, 14>; 

// Instruction Types
// INS_TYP Already uses InstructionType

// TODO loops

// Reference Types
using REF_SIZE = StrongTypedef<size_t, no_summable, 15>;
// REF_MTYP Already uses ReferenceType
using REF_NAME = std::string;

// Segment Types
// SEG_TYP Already uses SegmentType 

// Thread Types
using PID = StrongTypedef<unsigned, no_summable, 16>;
using TIME_STRING = std::string;
using NUM_CYCLES = StrongTypedef<unsigned, no_summable, 17>;

#endif /* TYPES_H_ */
