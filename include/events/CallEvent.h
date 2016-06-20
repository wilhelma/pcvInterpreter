/**
 *
 *    @file  CallEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_EVENT_H_
#define  CALL_EVENT_H_

#include "Types.h"
#include "ShadowThread.h"

typedef struct CallInfo {
    CALLSITE siteId;
    TIME runtime;
    FUN_SG fnSignature;
    SEG_ID segment;
    FunctionType fnType;
    FIL_PT fileName;
    FIL_PT filePath;

    explicit
        CallInfo(CALLSITE SiteId,
                TIME Runtime,
                FUN_SG FnSignature,
                SEG_ID Segment,
                FunctionType FnType,
                FIL_PT FileName,
                FIL_PT FilePath)
        : siteId(SiteId), runtime(Runtime), fnSignature(FnSignature),
        segment(Segment), fnType(FnType), fileName(FileName), filePath(FilePath) {}
} CallInfo;

class CallEvent : public Event {
    public:
        CallEvent(const ShadowThread *thread,
                const struct CallInfo *info) :
            Event(thread), _info(info) {}
        Events getEventType() const override { return Events::CALL; }
        const CallInfo* getCallInfo() const { return _info; };

    private:
        const struct CallInfo *_info;

        // prevent generated functions
        CallEvent(const CallEvent&);
        CallEvent& operator=(const CallEvent&);
};

#endif
