#include "ThreadMgr.h"

#include "ShadowThread.h"
#include "Types.h"

#include "easylogging++.h"

#include <memory>

// Helper function to create a new entry
std::unique_ptr<const ShadowThread> make_ShadowThread(const TRD_ID& thread_id) noexcept {
    return std::make_unique<const ShadowThread>(thread_id);
}

const std::shared_ptr<const ShadowThread> ThreadMgr::getThread(const TRD_ID& thread_id) noexcept {
    // Get the ShadowThread corresponding to thread_id
    const auto& thread_it = ThreadIdToShadowThread_.find(thread_id);
    if (thread_it != ThreadIdToShadowThread_.cend())
        return thread_it->second;

    // If the entry is not in the map, make a new one
    std::shared_ptr<const ShadowThread> thread = make_ShadowThread(thread_id);
    ThreadIdToShadowThread_.insert(std::make_pair(thread_id, thread)); 
    return thread;
}

void ThreadMgr::threadJoined(const TRD_ID& thread_id) {
	const auto& thread_it = ThreadIdToShadowThread_.find(thread_id);

    // In this case, the interpreter is probably buggy
    if (thread_it == ThreadIdToShadowThread_.cend()) {
        LOG(FATAL) << "Entry doesn't exist";
        throw 1;
    }

    // In this case, the pointer is still in use somewhere
    if (thread_it->second.unique()) {
        LOG(FATAL) << "Pointer used somewhere!";
        throw 2;
    }

    ThreadIdToShadowThread_.erase(thread_it);
}
