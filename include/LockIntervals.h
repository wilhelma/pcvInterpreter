#ifndef __LOCK_INTERVALS_H
#define __LOCK_INTERVALS_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "Types.h"

class LockInterval {
 public:
    TIME start;
    TIME stop;
    TIME span();
    void shift(TIME offset); 

    LockInterval(TIME s, TIME e)
        : start(s)
        , stop(e)
    {}
};

class LockIntervals {
 public:
 	void add(LockIntervals childIntervals);
 	void addInterval(TIME start, TIME end, unsigned int lock_id);
 	void clear();
 	void shift(TIME offset);
    TIME span();
    TIME waitTime();
 private:
 	void removeOverlaps();
 	std::unordered_map<unsigned int, std::vector<LockInterval>> interval_map;
 	std::vector<LockInterval> intervals;
 };

#endif
