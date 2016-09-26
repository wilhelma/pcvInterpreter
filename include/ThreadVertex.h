/**
 *
 *    @file  ThreadVertex.h
 *   @brief  Declarations for the class `ThreadVertex`
 *
 *    @date  09/26/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef THREADVERTEX
#define THREADVERTEX

class ThreadVertex {

 public:
	ThreadVertex();
	explicit ThreadVertex(TIME start, TIME span);
	~ThreadVertex();

 private:

 	TIME time;
 	int type;

	// prevent generated functions --------------------------------------------
	ThreadVertex& operator=(const ThreadVertex&);
	ThreadVertex(const ThreadVertex&);
};

#endif  // THREADVERTEX


