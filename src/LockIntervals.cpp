

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


TIME span(std::vector<LockInterval> intervls) {
    TIME sum = static_cast<TIME>(0);
    for (int i=0; i<intervls.size(); i++) {
        sum += intervls.at(i).span();
    }
    return sum;
}

std::vector<LockInterval> removeOverlaps(std::vector<LockInterval> intervls) {
    // Sort LockIntervals in decreasing order of
    // start time
    std::sort(intervls.begin(), intervls.end(), compareLockIntervalStarts);

    int index = 0; // Stores index of last element
    // in output intervalsay (modified intervals[])

    // Traverse all input LockIntervals
    for (int i=0; i<intervls.size(); i++)
    {
        // If this is not first LockInterval and overlaps
        // with the previous one
        if (index != 0 && intervls.at(index-1).start <= intervls.at(i).stop)
        {
            while (index != 0 && 
                   intervls.at(index-1).start <= intervls.at(i).stop)
            {
                // Merge previous and current LockIntervals
                intervls.at(index-1).stop = 
                    std::max(intervls.at(index-1).stop, intervls.at(i).stop);
                intervls.at(index-1).start = 
                 std::min(intervls.at(index-1).start, intervls.at(i).start);
                index--;
            }
        }
        else // Doesn't overlap with previous, add to
            // solution
            intervls.at(index) = intervls.at(i);

        index++;
    }
  
    return intervls;
}



TIME LockIntervals::waitTime() {

    TIME span_with_overlaps = span(intervals);
    TIME span_without_overlaps = span(removeOverlaps(intervals));
    printf("waitTime is %llu \n", (unsigned long long) 
                            (span_with_overlaps- span_without_overlaps));
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

void LockInterval::print() {
    std::cout << "AIN NEW INTERVA " << start << " " << stop << std::endl;
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
    newLockInterval.print();
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




