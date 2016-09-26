/**
 *
 *    @file  HorizontalThreadPlot.h
 *   @brief  Declarations for the class `HorizontalThreadPlot`
 *
 *    @date  09/26/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

 // UPDATE after every join event

#ifndef HORIZONTALTHREADPLOT
#define HORIZONTALTHREADPLOT

#include "Types.h"

struct ThreadEvnt {

	TIME time;
	int type;

	threadEvnt(TIME tme, int typ) : time(tme), type(typ) {}
};

class HorizontalThreadPlot {

 public:
	HorizontalThreadPlot();
	~HorizontalThreadPlot();
	end_segment(int level, TIME start, TIME stop);
	printJson();
	updateAfterSync();

 private:

 	vector<ThreadEvnt> sequential_thread_event_times;
 	vector<ThreadSegment> segment_stack;
 	std::map<int, vector<ThreadSegment>> levels;

	// prevent generated functions --------------------------------------------
	HorizontalThreadPlot& operator=(const HorizontalThreadPlot&);
	HorizontalThreadPlot(const HorizontalThreadPlot&);
	
};

#endif  // HORIZONTALTHREADPLOT


