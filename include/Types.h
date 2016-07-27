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

/// Struct to prevent implicit conversion among types.
/// @tparam T The type
/// @tparam n A dumb integer to prevent casts
/// @todo When used with `T = std::string`,
///// `string` methods cannot be called.
template <typename T, size_t n>
struct StrongTypedef {
	/// @brief Constructor.
	/// @details It's `explicit` in order to prevent implicit casts.
	constexpr explicit StrongTypedef(T value) noexcept
		: Value_(value)
		{}

	/// @brief Copy constructor
	constexpr StrongTypedef(const StrongTypedef<T,n>& input) noexcept
		: Value_(input.Value_)
		{}

	/// @brief Implicit conversion operator.
	/// @details A return by value is used sice I assume the
	/// underlying type should be a primitive.
	operator const T() const { return Value_; }

	/// \brief This is called when operator++ is
	/// called on the object.
	operator T&() { return Value_; }
private:
	T Value_;
};

// All others IDs should have the same type as ID
using ID     = unsigned;
using ACC_ID = StrongTypedef<ID,  0>;
using CAL_ID = StrongTypedef<ID,  1>;
using FIL_ID = StrongTypedef<ID,  2>;
using FUN_ID = StrongTypedef<ID,  3>;
using INS_ID = StrongTypedef<ID,  4>;
using LOP_ID = StrongTypedef<ID,  5>;
using LOE_ID = StrongTypedef<ID,  6>;
using LOI_ID = StrongTypedef<ID,  7>;
using REF_ID = StrongTypedef<ID,  8>;
using SEG_ID = StrongTypedef<ID,  9>;
using TRD_ID = StrongTypedef<ID, 10>;

// Access Types
using POS   = StrongTypedef<unsigned, 11>;
// ACC_TYP Already uses AccessType
// MEM_ST  Already uses AccessState

// Call Types
using TIME     = StrongTypedef<clock_t, 12>;
using CALLSITE = StrongTypedef<size_t,  13>;

// File Types
using FIL_PT = std::string;

// Function Types
using FUN_SG = std::string; 
// FUN_TYP Already uses FunctionType
using LIN_NO = StrongTypedef<unsigned, 14>; 

// Instruction Types
// INS_TYP Already uses InstructionType

// TODO loops

// Reference Types
using REF_SIZE = StrongTypedef<size_t, 15>;
// REF_MTYP Already uses ReferenceType
using REF_NAME = std::string;

// Segment Types
// SEG_TYP Already uses SegmentType 

// Thread Types
using PID = StrongTypedef<unsigned, 16>;
using TIME_STRING = std::string;
using NUM_CYCLES = StrongTypedef<unsigned, 16>;

#endif /* TYPES_H_ */
