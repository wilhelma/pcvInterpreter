#ifndef UTILITY_H_
#define UTILITY_H_

#define DEBUG_OUTPUT 0

static inline void print_time(std::string str, TIME time) {

	if (DEBUG_OUTPUT)
		std::cout << str << " " << time << std::endl;
}

#endif /* UTILITY_H_ */