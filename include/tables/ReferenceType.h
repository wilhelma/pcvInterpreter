/**
 *
 *    @file  ReferenceType.h
 *   @brief  
 *
 *    @date  01/13/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  REFERENCE_TYPE_H
#define  REFERENCE_TYPE_H

#include <iosfwd>

/// @ingroup types
/// @brief The type of the reference.
enum class ReferenceType : unsigned char {
    STACK  = (1 << 0), ///< Reference to a stack variable.
    HEAP   = (1 << 1), ///< Reference to a heap variable.
    STATIC = (1 << 2), ///< Reference to a static variable.
    GLOBAL = (1 << 3)  ///< Reference to a global variable.
};

/// @brief Output stream operator for ReferenceType.
/// @param s The output stream.
/// @param r The reference type to print.
/// @attention Remember to update this when changing ReferenceType!
std::ostream& operator<<(std::ostream& s, ReferenceType r);

#endif
