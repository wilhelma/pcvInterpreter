/**
 *
 *    @file  NewThreadEvent.cpp
 *   @brief  
 *
 *    @date  12/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "NewThreadEvent.h"
#include "NewThreadInfo.h"

#include <iomanip>
#include <ostream>

std::ostream& operator<<(std::ostream& s, const NewThreadEvent& e) {
    s << "Parent trd id: " << std::setw(3) << e.threadId() << " | ";
    s << "Child trd id:  " << std::setw(3) << e.info()->childThreadId() << " | ";
    s << "Start time:    " << std::setw(3) << e.info()->startTime() << " | ";
    s << "Runtime:       " << std::setw(2) << e.info()->runtime();
    return s;
}
