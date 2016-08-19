#ifndef UTILITY_H_
#define UTILITY_H_

#define DEBUG_OUTPUT 1
#include <exception>

#include "Types.h"

// TODO: write meto or combinin astables.

struct CallsiteNotInitializedException : public std::exception
{
	const char * what () const throw ()
	{
		return "CALLSITE not initialized";
	}
};

static inline void print_time(std::string str, TIME time) {

	if (DEBUG_OUTPUT)
		std::cout << str << " " << time << std::endl;
}

#endif /* UTILITY_H_ */