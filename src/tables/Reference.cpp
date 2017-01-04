/**
 *
 *    @file  Reference.cpp
 *   @brief  
 *
 *    @date  01/04/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "Reference.h"
#include <ostream>

std::ostream& operator<<(std::ostream& s, ReferenceType r) {
//    using underlying_type = std::underlying_type<ReferenceType>::type;
    using underlying_type = int;

    switch (r) {
        case ReferenceType::STACK:
            return s << static_cast<underlying_type>(r) << " (Stack)";
        case ReferenceType::HEAP:
            return s << static_cast<underlying_type>(r) << " (Heap)";
        case ReferenceType::STATIC:
            return s << static_cast<underlying_type>(r) << " (Static)";
        case ReferenceType::GLOBAL:
            return s << static_cast<underlying_type>(r) << " (Global)";
        default:
            return s;
    }
}
