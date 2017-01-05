/**
 *
 *    @file  ReturnEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ReturnEvent.h"

#include "EventType.h"
#include "ReturnInfo.h"

#include <iomanip>
#include <ostream>

EventType ReturnEvent::type() const noexcept
{ return EventType::RETURN; }

std::ostream& operator<<(std::ostream& s, const ReturnEvent& e) {
    s << "Event: "       << e.type() << " | ";
    s << "Thread id: "   << std::setw(2) << e.threadId() << " | ";
    s << "Call Id: "     << std::setw(2) << e.info()->callId() << " | ";
    s << "Call time: "   << std::setw(2) << e.info()->callTime() << " | ";
    s << "Return time: " << std::setw(2) << e.info()->runtime() << " | ";
    s << "Func id: "     << std::setw(2) << e.info()->functionId() << " | ";
    s << "Func type: "   << std::setw(2) << static_cast<unsigned>(e.info()->functionType()) << " | ";
    return s;
}
