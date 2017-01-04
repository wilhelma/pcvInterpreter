/**
 *
 *    @file  AccessEvent.cpp
 *   @brief  
 *
 *    @date  11/02/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AccessEvent.h"

#include "AccessInfo.h"
#include "EventType.h"
#include "ShadowThread.h"

#include <iomanip>
#include <ostream>

EventType AccessEvent::type() const noexcept
{ return EventType::ACCESS; }

std::ostream& operator<<(std::ostream& s, const AccessEvent& e) {
    s << "Instruction: " << std::setw(2) << e.info()->instructionId() << " | ";
    s << "Reference:   " << std::setw(2) << e.info()->variableId() << " | ";
    s << "AccessType:  " << std::setw(0) << e.info()->accessType() << " | ";
    s << "RefType:     " << std::setw(2) << static_cast<int>(e.info()->referenceType()) << " | ";
    s << "Size:        " << std::setw(2) << e.info()->size() << " | ";
    s << "Name:        " << std::setw(2) << e.info()->name();
    return s;
}
