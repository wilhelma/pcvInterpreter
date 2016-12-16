/**
 *
 *    @file  EventTestTool.h
 *   @brief  
 *
 *    @date  12/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  EVENT_TEST_TOOL_H_
#define  EVENT_TEST_TOOL_H_

#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/CallEvent.h"
#include "fwd/JoinEvent.h"
#include "fwd/NewThreadEvent.h"
#include "fwd/ReleaseEvent.h"
#include "fwd/ReturnEvent.h"
#include "fwd/ThreadEndEvent.h"

#include "Tool.h"

class EventTestTool final : public Tool {
public:

    /// Process AccessEvent.
    virtual void Access(const AccessEvent* e);
    /// Process AcquireEvent.
    virtual void Acquire(const AcquireEvent* e);
    /// Process CallEvent.
    virtual void Call(const CallEvent* e);
    /// Process JoinEvent.
    virtual void Join(const JoinEvent* e);
    /// Process NewThreadEvent.
    virtual void NewThread(const NewThreadEvent* e);
    /// Process ReleaseEvent.
    virtual void Release(const ReleaseEvent* e);
    /// Process ReturnEvent.
    virtual void Return(const ReturnEvent* e);
    /// Process ThreadEndEvent.
    virtual void ThreadEnd(const ThreadEndEvent* e);

    explicit EventTestTool() = default;
    ~EventTestTool() final   = default;

    unsigned int NumAccesses_   = 0;
    unsigned int NumJoins_      = 0;
    unsigned int NumNewThreads_ = 0;
    unsigned int NumThreadEnds_ = 0;
    unsigned int NumCalls_      = 0;
    unsigned int NumReturns_    = 0;
    unsigned int NumAcquires_   = 0;
    unsigned int NumReleases_   = 0;
};

#endif
