/**
 *
 *    @file  JoinEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "JoinEvent.h"

#include "EventType.h"
#include "JoinInfo.h"

#include <iomanip>
#include <ostream>

EventType JoinEvent::type() const noexcept
{ return EventType::JOIN; }

std::ostream& operator<<(std::ostream& s, const JoinEvent& e) {
    s << "Event:         " << std::setw(2) << e.type() << " | ";
    s << "Parent trd id: " << std::setw(3) << e.threadId() << " | ";
    s << "Child trd id:  " << std::setw(3) << e.info()->childThreadId() << " | ";
    s << "Start time:    " << std::setw(3) << e.info()->startTime() << " | ";
    s << "Join time:     " << std::setw(3) << e.info()->joinTime();
    return s;
}
