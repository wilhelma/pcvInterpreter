/**
 *
 *    @file  EventType.cpp
 *   @brief  
 *
 *    @date  01/04/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "EventType.h"

#include <ostream>

std::ostream& operator<<(std::ostream& s, EventType e) {
    switch (e) {
        case EventType::NEWTHREAD:
            return s << "NewThread";
        case EventType::THREADEND:
            return s << "ThreadEnd";
        case EventType::JOIN:
            return s << "Join";
        case EventType::ACQUIRE:
            return s << "Acquire";
        case EventType::RELEASE:
            return s << "Release";
        case EventType::ACCESS:
            return s << "Access";
        case EventType::CALL:
            return s << "Call";
        case EventType::RETURN:
            return s << "Return";
        default:
            return s;
    }
}
