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

#include <iostream>
#include <ostream>
#include <stdexcept>

// Helper function to determine how long the string is.
template <typename CharT>
constexpr const unsigned int string_length(const CharT* input, unsigned int i = 0) noexcept {
    return (input[i] == '\0' ? ++i : string_length(input, ++i));
}

// Helper function to allocate the memory for the string.
template <typename CharT>
constexpr const CharT* const save_string(const CharT* input, const unsigned int len) {
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
/// @todo Maybe use a shared_ptr to make sure there are no memory leaks.
template <typename CharT>
class FastString {
public:
    using difference_type = unsigned int;

//    /// @brief Constructor.
//    /// @details Compile-time template deduction is used to evaluate the array length.
//    /// @param  input The pointer to the first element of the string.
//    /// @tparam N     The length of the input character array.
//    /// @attention This is not `explicit` in order to allow implicit conversions from
//    /// arrays of characters to FastStrings at compile-time.
//    /// @throw std::bad_alloc If the memory for the string can't be allocated.
//    template <difference_type N>
//    constexpr FastString(const CharT (&input)[N]) :
//        Length_(N),
//        String_(save_string(input, Length_))
//    {}

    /// @brief Copy constructor.
     FastString(const FastString& fs) :
        Length_(fs.Length_),
        String_(save_string(fs.String_, Length_))
    {
//        std::cout << "Copied faststring: " << this << std::endl;
    }

    /// @brief Copy assignment operator.
    constexpr FastString& operator=(const FastString& fs) {
        if (this != &fs) {
            Length_ = fs.Length_;
            String_ = save_string(fs.String_, Length_);
        }

        return *this;
    }

    /// @brief Move constructor.
     FastString(FastString&& fs) noexcept :
        Length_(fs.Length_),
        String_(fs.String_)
    {
        // NOTE: String_ is nullptr only if one tries to
        // move a FastString that had been already moved.
//        std::cout << "Moved faststring:  " << this << std::endl;

        // Prevent fs's destructor from deallocating
        // the memory that this->String_ points to.
        fs.String_ = nullptr;
    }

    /// @brief Move assignment operator.
    constexpr FastString& operator=(FastString&& fs) noexcept { 
        if (this != &fs) {
            Length_ = fs.Length_;
            String_ = fs.String_;

            // Prevent fs's destructor from deallocating
            // the memory that this->String_ points to.
            fs.String_ = nullptr;
        }

        return *this;
    }

    /// @brief Destructor.
    /// @attention As soon as you have a non-trivial destructor, the
    /// class cannot be a literal type.
    ~FastString() {
        // Nothing will happen if String_ is nullptr.
        delete[] String_;
    }

    /// @brief Constructor.
    /// @attention The input string is assumed to be '\0'-terminated. If that's not
    /// the case, the behavior is undefined.
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

    /// _Default_ constructor.
    constexpr FastString() = default;

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
template <typename CharT1, typename CharT2>
constexpr const bool operator==(const FastString<CharT1>& lhs, const FastString<CharT2>& rhs) noexcept {
    if (lhs.length() != rhs.length())
        return false;

    for (typename FastString<CharT1>::difference_type i = 0; i < lhs.length(); ++ i)
        if (lhs[i] != rhs[i])
            return false;

    return true;
}

/// @brief Equality operator.
/// @param lhs The left-hand side FastString.
/// @param rhs The right-hand side character array.
template <typename CharT, size_t N>
inline constexpr const bool operator==(const FastString<CharT>& lhs, const char (&rhs)[N]) noexcept
{ return lhs == FastString<char>(std::forward<const char(&)[N]>(rhs)); }

/// @brief Unequality operator.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
template <typename CharT>
inline constexpr const bool operator!=(const FastString<CharT>& lhs, const FastString<CharT>& rhs) noexcept
{ return !( lhs == rhs ); }

/// @brief Output stream operator for FastString.
/// @param s  The output stream.
/// @param fs The FastString to stream.
template <typename CharT>
inline std::ostream& operator<<(std::ostream& s, const FastString<CharT>& fs)
{ return s << fs.c_str(); }

#endif
