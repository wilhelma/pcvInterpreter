/**
 *
 *    @file  AccessState.cpp
 *   @brief  
 *
 *    @date  01/13/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AccessState.h"
#include <ostream>

std::ostream& operator<<(std::ostream& s, AccessState a) {
    using underlying_type = int;

    switch (a) {
        case AccessState::INIT:
            return s << static_cast<underlying_type>(a) << " (Init)";
        case AccessState::EXCLUSIVE:
            return s << static_cast<underlying_type>(a) << " (Exclusive)";
        case AccessState::READ_SHARED:
            return s << static_cast<underlying_type>(a) << " (ReadShared)";
        case AccessState::READ_WRITE_SHARED:
            return s << static_cast<underlying_type>(a) << " (ReadWriteShared)";
        default:
            return s;
    }
}
