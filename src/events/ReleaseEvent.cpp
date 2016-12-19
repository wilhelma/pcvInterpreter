/**
 *
 *    @file  ReleaseEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ReleaseEvent.h"

#include "EventType.h"
#include "ReleaseInfo.h"

#include <iomanip>
#include <ostream>

EventType ReleaseEvent::type() const noexcept
{ return EventType::RELEASE; }

std::ostream& operator<<(std::ostream& s, const ReleaseEvent& e) {
    s << "Event:        " << std::setw(2) << e.type() << " | ";
    s << "Thread id:    " << std::setw(2) << e.threadId() << " | ";
    s << "Lock id:      " << std::setw(2) << e.info()->lockId() << " | ";
    s << "Acquire time: " << std::setw(2) << e.info()->acquireTime();
    s << "Release time: " << std::setw(2) << e.info()->releaseTime();
    return s;
}
