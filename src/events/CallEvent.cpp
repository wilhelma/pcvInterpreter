/**
 *
 *    @file  CallEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "CallEvent.h"
#include "CallInfo.h"
#include "EventType.h"

#include <iomanip>
#include <ostream>

EventType CallEvent::type() const noexcept
{ return EventType::CALL; }

std::ostream& operator<<(std::ostream& s, const CallEvent& e) {
    s << "Event: " << e.type() << " | ";
    s << "Thread id:  " << std::setw(2) << e.threadId() << " | ";
    s << "Call Id:    " << std::setw(2) << e.info()->callId() << " | ";
    s << "Call time:  " << std::setw(2) << e.info()->callTime() << " | ";
    s << "Call Site:  " << e.info()->siteId() << " | ";
    s << "Runtime:    " << std::setw(2) << e.info()->runtime() << " | ";
    s << "Func id:    " << std::setw(2) << e.info()->functionId() << " | ";
    s << "Func type:  " << std::setw(2) << static_cast<unsigned>(e.info()->functionType()) << " | ";
	s << "Func sig:   " << e.info()->functionSignature();
    return s;
}
