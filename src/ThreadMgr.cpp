#include "ThreadMgr.h"

#include "ShadowThread.h"
#include "ThreadManagerException.h"
#include "Types.h"

#include "easylogging++.h"

#include <memory>

const std::shared_ptr<const ShadowThread> ThreadMgr::getThread(const TRD_ID& thread_id) noexcept {
    // Get the ShadowThread corresponding to thread_id
    const auto& thread_it = ThreadIdToShadowThread_.find(thread_id);
    if (thread_it != ThreadIdToShadowThread_.cend())
        return thread_it->second;

    // If the entry is not in the map, make a new one
    std::shared_ptr<const ShadowThread> thread = std::make_unique<const ShadowThread>(thread_id);
    ThreadIdToShadowThread_.insert(std::make_pair(thread_id, thread)); 
    return thread;
}

void ThreadMgr::threadJoined(const TRD_ID& thread_id) {
    const auto& thread_it = ThreadIdToShadowThread_.find(thread_id);

    // In this case, the interpreter is probably buggy
    if (thread_it == ThreadIdToShadowThread_.cend())
        throw ThreadManagerException("Entry " + std::to_string(thread_id) + " not found",
                                     "ThreadMgr::threadJoined");

    // In this case, the pointer is still in use somewhere
    if (!thread_it->second.unique())
        throw ThreadManagerException("ShadowThread pointer to thread " + std::to_string(thread_id) + " is still in use",
                                     "ThreadMgr::threadJoined");

    ThreadIdToShadowThread_.erase(thread_it);
}
