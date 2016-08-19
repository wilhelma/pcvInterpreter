#ifndef UTILITY_H_
#define UTILITY_H_

#define DEBUG_OUTPUT 1
#include "Types.h"

// TODO: write meto or combinin astables.

static inline void print_time(std::string str, TIME time) {

	if (DEBUG_OUTPUT)
		std::cout << str << " " << time << std::endl;
}

#endif /* UTILITY_H_ */