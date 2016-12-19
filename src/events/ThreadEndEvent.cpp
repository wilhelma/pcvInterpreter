/**
 *
 *    @file  ThreadEndEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ThreadEndEvent.h"

#include "EventType.h"
#include "ThreadEndInfo.h"

#include <iomanip>
#include <ostream>

EventType ThreadEndEvent::type() const noexcept
{ return EventType::THREADEND; }

std::ostream& operator<<(std::ostream& s, const ThreadEndEvent& e) {
    s << "Event:         " << std::setw(3) << e.type() << " | ";
    s << "Parent trd id: " << std::setw(3) << e.threadId() << " | ";
    s << "Child trd id:  " << std::setw(3) << e.info()->childThreadId() << " | ";
    s << "Start time:    " << std::setw(3) << e.info()->startTime() << " | ";
    s << "End time:      " << std::setw(2) << e.info()->endTime();
    return s;
}
