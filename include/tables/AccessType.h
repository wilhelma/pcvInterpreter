/**
 *
 *    @file  AccessType.h
 *   @brief  
 *
 *    @date  01/13/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_TYPE_H
#define  ACCESS_TYPE_H

#include <iosfwd>

/// @ingroup types
/// @brief The type of the access.
enum class AccessType : unsigned char {
	READ  = 1, ///< Read access.
	WRITE = 2  ///< Write access.
};

/// @brief Output stream operator for AccessType.
/// @param s The output stream.
/// @param a The access type to print.
/// @attention Remember to update this when changing AccessType!
std::ostream& operator<<(std::ostream& s, AccessType a);

#endif
