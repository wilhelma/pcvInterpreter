/**
 *
 *    @file  AcquireEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AcquireEvent.h"
#include "AcquireInfo.h"
#include "EventType.h"

#include <iomanip>
#include <ostream>

EventType AcquireEvent::type() const noexcept
{ return EventType::ACQUIRE; }

std::ostream& operator<<(std::ostream& s, const AcquireEvent& e) {
    s << "Event:        " << std::setw(2) << e.type() << " | ";
    s << "Thread id:    " << std::setw(2) << e.threadId() << " | ";
    s << "Lock id:      " << std::setw(2) << e.info()->lockId() << " | ";
    s << "Acquire time: " << std::setw(2) << e.info()->acquireTime();
    return s;
}
