/**
 *
 *    @file  EventTestTool.cxx
 *   @brief  
 *
 *    @date  12/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "./EventTestTool.h"

// -- Events -----------------
#include "AccessEvent.h"
#include "AcquireEvent.h"
#include "CallEvent.h"
#include "JoinEvent.h"
#include "NewThreadEvent.h"
#include "ReleaseEvent.h"
#include "ReturnEvent.h"
#include "ThreadEndEvent.h"
// -- Infos ------------------
#include "AccessInfo.h"
#include "AcquireInfo.h"
#include "CallInfo.h"
#include "JoinInfo.h"
#include "NewThreadInfo.h"
#include "ReleaseInfo.h"
#include "ReturnInfo.h"
#include "ThreadEndInfo.h"
// ---------------------------

#include "gtest/gtest.h"

#include <iomanip>

std::ostream& operator<<(std::ostream& s, const AccessEvent& e) {
//           s << " ------------ \n";
           s << "Instruction: " << std::setw(2) << e.info()->instructionId() << " | ";
           s << "Reference:   " << std::setw(2) << e.info()->variableId() << " | ";
           s << "AccessType:  " << std::setw(2) << static_cast<int>(e.info()->accessType()) << " | ";
           s << "RefType:     " << std::setw(2) << static_cast<int>(e.info()->referenceType()) << " | ";
           s << "Size:        " << std::setw(2) << e.info()->size() << " | ";
    return s << "Name:        " << std::setw(2) << e.info()->name() << " | ";
}

void EventTestTool::Access(const AccessEvent* e) {
    ++ NumAccesses_;

    // Check correctness of the information in the event
    if (e->info()->instructionId() == 19) {
        EXPECT_EQ(e->info()->variableId(),    6);
        EXPECT_EQ(e->info()->referenceType(), ReferenceType::STACK);
        EXPECT_EQ(e->info()->accessType(),    AccessType::READ);
        EXPECT_EQ(e->info()->size(),          8);
        EXPECT_EQ(e->info()->name(),          "val");

        std::cout << *e << std::endl;
    } else if (e->info()->instructionId() == 35) {
        EXPECT_EQ(e->info()->variableId(),    12);
        EXPECT_EQ(e->info()->referenceType(), ReferenceType::STATICVAR);
        EXPECT_EQ(e->info()->accessType(),    AccessType::READ);
        EXPECT_EQ(e->info()->size(),          4);
        EXPECT_EQ(e->info()->name(),          "global");

        std::cout << *e << std::endl;
    } else if (e->info()->instructionId() == 45) {
        EXPECT_EQ(e->info()->variableId(),    4);
        EXPECT_EQ(e->info()->referenceType(), ReferenceType::HEAP);
        EXPECT_EQ(e->info()->accessType(),    AccessType::WRITE);
        EXPECT_EQ(e->info()->size(),          4);
        EXPECT_EQ(e->info()->name(),          "26394640");

        std::cout << *e << std::endl;
    }
}

void EventTestTool::Acquire(const AcquireEvent* e) {
    ++ NumAcquires_;
}

void EventTestTool::Call(const CallEvent* e) {
    ++ NumCalls_;
}

void EventTestTool::Join(const JoinEvent* e) {
    EXPECT_LE(e->threadId(), e->info()->childThreadId());
    ++ NumJoins_;
}

void EventTestTool::NewThread(const NewThreadEvent* e) {
    EXPECT_LE(e->threadId(), e->info()->childThreadId());

    ++ NumNewThreads_;

//    std::cout << "New Thread" << std::endl;
//    std::cout << " Parent id:   " << e->threadId() << std::endl;
//    std::cout << " Child id:    " << e->info()->childThreadId() << std::endl;
//    std::cout << " Start time:  " << e->info()->startTime() << std::endl;
//    std::cout << " Run time:    " << e->info()->runTime() << std::endl;
}

void EventTestTool::Release(const ReleaseEvent* e) {
    ++ NumReleases_;
}

void EventTestTool::Return(const ReturnEvent* e) {
    ++ NumReturns_;
}

void EventTestTool::ThreadEnd(const ThreadEndEvent* e) {
    EXPECT_LE(e->threadId(), e->info()->childThreadId());

    ++ NumThreadEnds_;
}
