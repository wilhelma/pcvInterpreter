/**
 *
 *    @file  ThreadStack.h
 *   @brief  Declarations for the BackeryLock and BakeryLocks classes. 
 *
 *    @date  12/08/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 */

#ifndef __BAKERY_LOCK_H
#define __BAKERY_LOCK_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "Types.h"

class BackeryLock {

 public:
 	void lock();
 	void unlock();
 private:
 	std::vector<bool> waiting_flag;
 	std::vector<int> order_label;
};

class BackeryLocks {
 public:
 private:

 };

#endif
