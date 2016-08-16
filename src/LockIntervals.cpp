

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

TIME getMinStart(std::shared_ptr<std::vector<LockInterval>> intervls) {

    TIME min = static_cast<TIME>(0);
    for (int i = 0; i < intervls->size(); i++) {
        if (intervls->at(i).start < min)
            min = intervls->at(i).start;
    }
    return min;
};

TIME getMaxStop(std::shared_ptr<std::vector<LockInterval>> intervls) {

    TIME max = static_cast<TIME>(0);
    for (int i = 0; i < intervls->size(); i++) {
        if (intervls->at(i).stop > max)
            max = intervls->at(i).stop;
    }
    return max;
};

TIME span(std::shared_ptr<std::vector<LockInterval>> intervls) {

    TIME start = getMinStart(intervls);
    TIME stop =  getMaxStop(intervls);
    assert(stop > start);
    TIME span = static_cast<TIME>(stop - start);
    return span;
}


LockIntervals::LockIntervals() {
    intervals = std::shared_ptr<std::vector<LockInterval>>(new std::vector<LockInterval>());
    ordered_intervals = std::shared_ptr<std::vector<LockInterval>>(new std::vector<LockInterval>());
}

void LockIntervals::order() {

    ordered_intervals = std::shared_ptr<std::vector<LockInterval>>(
                             new std::vector<LockInterval>(*(intervals.get())));
    std::sort(ordered_intervals->begin(), ordered_intervals->end(), compareLockIntervalStarts);

    for (int i = 0; i < ordered_intervals->size() - 1; i++) {

        if (ordered_intervals->at(i).stop > ordered_intervals->at(i+1).start) {
            TIME right_shift = static_cast<TIME>(1 + ordered_intervals->at(i).stop - 
                                                 ordered_intervals->at(i+1).start);
            ordered_intervals->at(i+1).shift(right_shift);
        }
    }
}

TIME LockIntervals::waitTime() {

    TIME unordered_span = span(intervals);
    printf("unordered_span is %llu \n", (unsigned long long)
                              static_cast<TIME>(unordered_span));
    order();
    TIME ordered_span = span(ordered_intervals);
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




