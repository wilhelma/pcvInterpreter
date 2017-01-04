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
#include "Reference.h"
#include "ShadowThread.h"

#include <iomanip>
#include <ostream>

EventType AccessEvent::type() const noexcept
{ return EventType::ACCESS; }

std::ostream& operator<<(std::ostream& s, const AccessEvent& e) {
    s << "Event: " << e.type() << " | ";
    s << "Instruction: " << std::setw(4) << e.info()->instructionId() << " | ";
    s << "Reference: "   << std::setw(4) << e.info()->variableId() << " | ";
    s << "AccessType: "  << e.info()->accessType() << " | ";
    s << "RefType: "     << e.info()->referenceType() << " | ";
    s << "Size: "        << std::setw(2) << e.info()->size() << " | ";
    s << "Name: "        << e.info()->name();
    return s;
}
