/**
 *
 *    @file  AccessState.h
 *   @brief  
 *
 *    @date  01/13/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_STATE_H
#define  ACCESS_STATE_H

#include <iosfwd>

/// @ingroup types
/// @brief The state of the access.
enum class AccessState : unsigned char {
    INIT              = 0,
    EXCLUSIVE         = 1,
    READ_SHARED       = 2,
    READ_WRITE_SHARED = 3
};

/// @brief Output stream operator for AccessState.
/// @param s The output stream.
/// @param a The access state to print.
/// @attention Remember to update this when changing AccessState!
std::ostream& operator<<(std::ostream& s, AccessState a);

#endif
