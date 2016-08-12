

/**
 *
 *    @file  LockIntervals.cpp
 *   @brief  Implementation file for the classes `LockInterval` and LockIntervalTree
 *
 *    @date  08/11/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

 // https://en.wikipedia.org/wiki/Lamport%27s_bakery_algorithm

 #include "LockIntervals.h"

// sort using a custom function object
struct {
    bool operator()(LockInterval a, LockInterval b)
    {   
        return a.start > b.start;
    }   
} compareLockIntervalStarts;


TIME LockIntervals::waitTime() {

    TIME span_with_overlaps = span();
    removeOverlaps();
    TIME span_without_overlaps = span();
    return static_cast<TIME>(span_with_overlaps- span_without_overlaps);
}


void LockIntervals::add(LockIntervals childIntervals) {

    intervals.insert(intervals.end(),
                     childIntervals.intervals.begin(),
                     childIntervals.intervals.end());

    for(auto it : childIntervals.interval_map) {
        unsigned int k = it.first;
        if (!interval_map.count(k)) {
            std::vector<LockInterval> newVector;
            interval_map.insert(std::pair<unsigned int, std::vector<LockInterval>>
                                (k, newVector));
        }

        interval_map.at(k).insert(interval_map.at(k).end(),
                                  childIntervals.interval_map.at(k).begin(),
                                  childIntervals.interval_map.at(k).end());
    }
}


void LockIntervals::addInterval(TIME start, TIME end, unsigned int lock_id) {

    LockInterval newLockInterval(start, end);
    intervals.push_back(newLockInterval);
    if (interval_map.count(lock_id))
        interval_map.at(lock_id).push_back(newLockInterval);
    else {
        std::vector<LockInterval> newVector;
        newVector.push_back(newLockInterval);
        interval_map.insert(std::pair<unsigned int, std::vector<LockInterval>>
                            (lock_id, newVector));
    }
}


void LockIntervals::clear() {
    intervals.clear();
    for (auto it : interval_map) {
        it.second.clear();
    }
}


void LockInterval::shift(TIME offset) {
    start += offset;
    stop += offset;
}

void LockIntervals::shift(TIME offset) {
    for (int i = 0; i < intervals.size(); i++) {
        intervals.at(i).shift(offset);
    }
}

TIME LockInterval::span() {
    return static_cast<TIME>(stop - start);
}

void LockIntervals::removeOverlaps() {
    // Sort LockIntervals in decreasing order of
    // start time
    std::sort(intervals.begin(), intervals.end(), compareLockIntervalStarts);

    int index = 0; // Stores index of last element
    // in output intervalsay (modified intervals[])

    // Traverse all input LockIntervals
    for (int i=0; i<intervals.size(); i++)
    {
        // If this is not first LockInterval and overlaps
        // with the previous one
        if (index != 0 && intervals.at(index-1).start <= intervals.at(i).stop)
        {
            while (index != 0 && 
                   intervals.at(index-1).start <= intervals.at(i).stop)
            {
                // Merge previous and current LockIntervals
                intervals.at(index-1).stop = 
                    std::max(intervals.at(index-1).stop, intervals.at(i).stop);
                intervals.at(index-1).start = 
                 std::min(intervals.at(index-1).start, intervals.at(i).start);
                index--;
            }
        }
        else // Doesn't overlap with previous, add to
            // solution
            intervals.at(index) = intervals.at(i);

        index++;
    }

    // // Now intervals[0..index-1] stores the merged LockIntervals
    // cout << "\n The Merged LockIntervals are: ";
    // for (int i = 0; i < index; i++)
    //     cout << "[" << intervals.at(i).start << ", " << intervals.at(i).stop << "] ";
}

TIME LockIntervals::span() {
    TIME sum = static_cast<TIME>(0);
    for (int i=0; i<intervals.size(); i++) {
        sum += intervals.at(i).span();
    }
    return sum;
}

