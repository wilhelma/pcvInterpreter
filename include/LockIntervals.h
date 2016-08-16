#ifndef __LOCK_INTERVALS_H
#define __LOCK_INTERVALS_H

#include <vector>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "Types.h"
#include "Utility.h"

class LockInterval {
 public:
    TIME start;
    TIME stop;
    void print();
    TIME span();
    void shift(TIME offset); 

    LockInterval(TIME s, TIME e)
        : start(s)
        , stop(e)
    {}
};

class LockIntervals {
 public:
 	LockIntervals();
 	void add(LockIntervals childIntervals);
 	void addInterval(TIME start, TIME end, unsigned int lock_id);
 	void clear();
 	void order();
 	void print(std::shared_ptr<std::vector<LockInterval>> intervls);
 	void print();
 	TIME waitTime();
 private:
 	std::unordered_map<unsigned int, std::vector<LockInterval>> interval_map;
 	std::shared_ptr<std::vector<LockInterval>> intervals;
 	std::shared_ptr<std::vector<LockInterval>> ordered_intervals;
 };

#endif
