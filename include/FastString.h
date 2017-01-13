/**
 *
 *    @file  FastString.h
 *   @brief  
 *
 *    @date  01/13/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FAST_STRING_H
#define  FAST_STRING_H

#include <ostream>
#include <stdexcept>

// Helper function to determine how long the string is.
template <typename CharT>
inline constexpr unsigned int string_length(const CharT* input, unsigned int i = 0) noexcept {
    return (input[i] == '\0' ? ++i : string_length(input, ++i));
}

// Helper function to allocate the memory for the string.
template <typename CharT>
inline constexpr const CharT* const save_string(const CharT* input, const unsigned int len) {
    // Dynamically allocate the string to prevent the memory from being freed
    // at the end of the function scope
    auto tmp = new CharT[len];

    // Copy the string
    for (unsigned int i = 0; i < len; ++ i)
        tmp[i] = input[i];

    return tmp;
}

/// @ingroup types
/// @brief Lightweight literal string to replace `std::string`.
template <typename CharT>
class FastString {
public:
    using difference_type = unsigned int;

    /// @brief Constructor.
    /// @details Compile-time template deduction is used to evaluate the array length.
    /// @param  input The pointer to the first element of the string.
    /// @tparam N     The length of the input character array.
    /// @throw std::bad_alloc If the memory for the string can't be allocated.
    template <difference_type N>
    constexpr explicit FastString(const CharT input[N]) :
        Length_(N),
        String_(save_string(input, Length_))
    {}

    /// @brief Constructor.
    /// @param input The pointer to a character array to store in the FastString.
    /// @throw std::invalid_argument If `input` is `nullptr`.
    /// @throw std::bad_alloc        If the memory for the string can't be allocated.
    constexpr explicit FastString(const CharT* input) :
        Length_( (!input) ? 0 : string_length(input) ),
        String_( (!input) ? nullptr : save_string(input, Length_) )
    {
        if (!String_)
            throw std::invalid_argument("Input string is nullptr from FastString::FastString");
    }

    /// Return the i-th element (without boundary checking).
    constexpr const CharT operator[](const difference_type i) const noexcept
    { return String_[i]; }

    /// Returns a pointer to a C-like string with the same content as the FastString. 
    constexpr const CharT* const c_str() const noexcept
    { return String_; }

    /// Returns the length of the FastString (excluding the terminator).
    constexpr const difference_type length() const noexcept
    { return Length_ - 1; }

private:
    /// The length of the FastString (includes the `\0` terminator).
    difference_type Length_;
    /// The actual string content.
    const CharT* String_;
};

/// @brief Equality operator.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
template <typename CharT>
constexpr const bool operator==(const FastString<CharT> lhs, const FastString<CharT> rhs) noexcept {
    if (lhs.length() != rhs.length())
        return false;

    for (typename FastString<CharT>::difference_type i = 0; i < lhs.length(); ++ i)
        if (lhs[i] != rhs[i])
            return false;

    return true;
}

/// @brief Equality operator.
/// @param lhs The left-hand side FastString.
/// @param rhs The right-hand side character array.
template <typename CharT>
constexpr const bool operator==(const FastString<CharT> lhs, const char* rhs) noexcept {
    return lhs == FastString<CharT>(reinterpret_cast<const CharT*>(rhs));
}

/// @brief Unequality operator.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
template <typename CharT>
constexpr const bool operator!=(const FastString<CharT> lhs, const FastString<CharT> rhs) noexcept
{ return !( lhs == rhs ); }

/// @brief Output stream operator for FastString.
/// @param s  The output stream.
/// @param fs The FastString to stream.
template <typename CharT>
std::ostream& operator<<(std::ostream& s, const FastString<CharT>& fs) {
    return s << fs.c_str();
}

#endif
