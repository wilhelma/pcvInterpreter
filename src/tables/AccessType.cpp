/**
 *
 *    @file  AccessType.cpp
 *   @brief  
 *
 *    @date  01/04/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AccessType.h"
#include <ostream>

std::ostream& operator<<(std::ostream& s, AccessType a) {
    using underlying_type = int;

    switch (a) {
        case AccessType::READ:
            return s << static_cast<underlying_type>(a) << " (Read)";
        case AccessType::WRITE:
            return s << static_cast<underlying_type>(a) << " (Write)";
        default:
            return s;
    }
}
