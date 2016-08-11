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

 #include "LockIntervals.h"

// sort using a custom function object
struct {
    bool operator()(LockInterval a, LockInterval b)
    {   
        return a.start > b.start;
    }   
} compareLockIntervalStarts;

void LockIntervals::add(TIME start, TIME end, unsigned int lock_id) {

    LockInterval newLockInterval(start, end);
    intervals.push_back(newLockInterval);
    if ()
        interval_map.insert(lock_id, newLockInterval);
    else 
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

    removeOverlaps();
    TIME sum = static_cast<TIME>(0);
    for (int i=0; i<intervals.size(); i++) {
        sum += intervals.at(i).span();
    }
    return sum;
}

