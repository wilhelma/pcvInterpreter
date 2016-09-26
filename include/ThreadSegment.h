/**
 *
 *    @file  ThreadSegment.h
 *   @brief  Declarations for the class `ThreadSegment`
 *
 *    @date  09/26/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef THREADSEGMENT
#define THREADSEGMENT

class ThreadSegment {

 public:
	ThreadSegment();
	explicit ThreadSegment(TIME start);
	~ThreadSegment();
	shiftLeft(TIME offset);
	end_segment(TIME stop, TIME span);
	
 private:

 	TIME start;
 	TIME stop;
 	TIME span;

	// prevent generated functions --------------------------------------------
	ThreadSegment& operator=(const ThreadSegment&);
	ThreadSegment(const ThreadSegment&);
};

#endif  // THREADSEGMENT


