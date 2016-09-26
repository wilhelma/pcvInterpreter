/**
 *
 *    @file  ThreadStack.h
 *   @brief  Declarations for the thread_frame_t struct and 
 			 the ThreadStack class.
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef ThreadStack_H_
#define ThreadStack_H_

#include <vector> 

#include "Span.h"
#include "DAG.h"
#include "LockIntervals.h"
#include "Types.h"
#include "Utility.h"

/**
*    @struct thread_frame_t
*    @brief Thread frame providing information about one thread, updated with events.
*/
struct thread_frame_t {

	/**
	*    @var thread
	*    @brief The thread's ID.
	*/
	TRD_ID thread;

	/**
	*    @var head_function_index
	*    @brief The index of the thread's head function (the first function called 
				in the thread after it spawns), in the function_stack of main_stack.
	*/
	int head_function_index;

	LockIntervals child_lock_intervals;
	LockIntervals lock_intervals;

	void absorb_child_locks() {
		lock_intervals.add(child_lock_intervals);
		child_lock_intervals.clear();
	}

	void add_child_locks(std::shared_ptr<thread_frame_t> child_thread) {
		child_lock_intervals.add(child_thread->lock_intervals);
	}

	vertex_descr_type first_vertex;
	vertex_descr_type last_vertex;

	std::list<vertex_descr_type> join_vertex_list;

	/**
	*    @var prefix
	*    @brief Prefix data for each call site in this thread.
	*/
	Span prefix;

	/**
	*    @var longest_child
	*    @brief Longest child data for each call site in this thread. 
	*/
	Span longest_child;

	/**
	*    @var continuation
	*    @brief Continuation data for each call site in this thread.
	*/
	Span continuation;

	int spawned_children_count;

	TIME current_edge_length;

	thread_frame_t():thread(static_cast<TIME>(0)),
					 prefix(Span()), 
					 longest_child(Span()),
					 continuation(Span()),
					 current_edge_length(static_cast<TIME>(0)) {}
};

class ThreadStack {

	public:
		
		ThreadStack();
		~ThreadStack();

		/**
		*    @fn init_frame()
		*    @brief Initializes a thread frame with index thread_index in the 
					thread stack, and head function index head_function_index.
		*/
		void init_frame(int thread_index, int head_function_index, 
										  TRD_ID thread,
										  vertex_descr_type first_vertex);

		/**
		*    @fn push()
		*    @brief Pushes a new thread_frame_t onto the thread stack 
					vector.
		*/
		std::shared_ptr<thread_frame_t> push(int head_function_index,
											 TRD_ID thread_id,
											 vertex_descr_type first_vertex);

		/**
		*    @fn pop()
		*    @brief Pops a thread_frame_t off of the thread stack 
					vector.
		*/
		void pop();

  		/**
		*    @var thread_stack
		*    @brief Thread stack that tracks bottom function in simulator,
				    implemented as a vector of std::shared_ptr<thread_frame_t>.
		*/
  		std::vector< std::shared_ptr<thread_frame_t> > stack;

 	private:

  		ThreadStack(const ThreadStack&);
		ThreadStack& operator=(const ThreadStack&);
};


#endif  // ThreadStack_H_ 
