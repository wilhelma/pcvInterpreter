

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


TIME span(std::shared_ptr<std::vector<LockInterval>> intervls) {
    TIME sum = static_cast<TIME>(0);
    for (int i=0; i<intervls->size(); i++) {
        sum += intervls->at(i).span();
    }
    return sum;
}


LockIntervals::LockIntervals() {
    intervals = std::shared_ptr<std::vector<LockInterval>>(new std::vector<LockInterval>());
    ordered_intervals = std::shared_ptr<std::vector<LockInterval>>(new std::vector<LockInterval>());
}

void LockIntervals::order() {

    ordered_intervals = intervals;
    std::sort(ordered_intervals->begin(), ordered_intervals->end(), compareLockIntervalStarts);
    TIME interval_strt = static_cast<TIME>(ordered_intervals->at(0).start - 1);

    for (auto it: *ordered_intervals) {

        it.shift(static_cast<TIME>(interval_strt - it.start));
        interval_strt = static_cast<TIME>(it.stop + 1);
    }
}

TIME LockIntervals::waitTime() {

    TIME unordered_span = span(intervals);
    printf("unordered_span is %llu \n", (unsigned long long)
                              static_cast<TIME>(unordered_span));
    order();
    print();
    TIME ordered_span = static_cast<TIME>(ordered_intervals->at(0).stop - 
                                          ordered_intervals->at(ordered_intervals->size() - 1).start);
    printf("ordered_span is %llu \n", (unsigned long long)
                        static_cast<TIME>(ordered_span));
    printf("waitTime is %llu \n", (unsigned long long)
                              static_cast<TIME>(ordered_span - unordered_span));
    return static_cast<TIME>(ordered_span - unordered_span);;
}


void LockIntervals::add(LockIntervals childIntervals) {

    intervals->insert(intervals->end(),
                     childIntervals.intervals->begin(),
                     childIntervals.intervals->end());

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

void LockIntervals::print(std::shared_ptr<std::vector<LockInterval>> intervls) { 

    for (auto it: *intervls) {
        it.print();
    }
}

void LockIntervals::print() {
    printf("intervals are now \n");
    print(intervals);
    printf("ordered intervals are now \n");
    print(ordered_intervals);
}

void LockInterval::print() {
    std::cout << "interval " << start << " " << stop << std::endl;
}

void LockIntervals::addInterval(TIME start, TIME end, unsigned int lock_id) {

    LockInterval newLockInterval(start, end);
    intervals->push_back(newLockInterval);
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
    intervals->clear();
    for (auto it : interval_map) {
        it.second.clear();
    }
}

void LockInterval::shift(TIME offset) {
    start += offset;
    stop += offset;
}


TIME LockInterval::span() {
    return static_cast<TIME>(stop - start);
}




