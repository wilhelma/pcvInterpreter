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

#include "CallSiteSpanHashtable.h"
#include "DAG.h"
#include "Intervals.h"
#include "Types.h"

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

	/**
	*    @var continuation_span
	*    @brief Continuation span of this thread. 
	*/
	TIME continuation_span;


 	/**
	*    @var prefix_span
	*    @brief Prefix span of this thread. 
	*/
	TIME prefix_span;

	Intervals child_lock_intervals;
	Intervals lock_intervals;

	TIME lock_span() {
		return lock_intervals.span();
	}

	void absorb_child_locks() {
		lock_intervals.insert(lock_intervals.end(),
							  child_lock_intervals.begin(),
				 		      child_lock_intervals.end());
	}

	void add_child_locks(thread_frame_t child_thread) {

		TIME offset = child_thread->start_time;
		child_thread->make_lock_interval_times_concurrent();
		child_lock_intervals.insert(child_lock_intervals.end(),
									child_thread->lock_intervals.begin(),
						 		    child_thread->lock_intervals.end());
	}

	/**
	*    @var lock_span
	*    @brief Span of the longest spawned child of this thread.
	*/
	TIME longest_child_span;

	/**
	*    @var longest_child_lock_span
	*    @brief Lock span of the longest spawned child of this thread.
	*/
	TIME longest_child_lock_span;

	TIME thread_start_time;

	vertex_descr_type first_vertex;
	vertex_descr_type last_vertex;

	std::list<vertex_descr_type> join_vertex_list;

	/**
	*    @var prefix_table
	*    @brief Prefix data for each call site in this thread.
	*/
	CallSiteSpanHashtable prefix_table;

	/**
	*    @var longest_child_table
	*    @brief Longest child data for each call site in this thread. 
	*/
	CallSiteSpanHashtable longest_child_table;

	/**
	*    @var continuation_table
	*    @brief Continuation data for each call site in this thread.
	*/
	CallSiteSpanHashtable continuation_table;

	thread_frame_t():thread(0),
					 continuation_span(0),
					 prefix_span(0), 
					 longest_child_span(0),
					 longest_child_lock_span(0), 
					 thread_start_time(0),
					 prefix_table(CallSiteSpanHashtable()), 
					 longest_child_table(CallSiteSpanHashtable()),
					 continuation_table(CallSiteSpanHashtable()) {}
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
