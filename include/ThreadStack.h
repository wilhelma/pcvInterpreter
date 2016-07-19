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

#include "CallSiteHashtable.h"
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


	TIME last_longest_child_span;

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


	/**
	*    @var lock_span
	*    @brief Lock span of this thread. 
	*/
	TIME lock_span;


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


	/**
	*    @var prefix_table
	*    @brief Prefix data for each call site in this thread.
	*/
	CallSiteHashtable prefix_table;

	/**
	*    @var longest_child_table
	*    @brief Longest child data for each call site in this thread. 
	*/
	CallSiteHashtable longest_child_table;

	/**
	*    @var continuation_table
	*    @brief Continuation data for each call site in this thread.
	*/
	CallSiteHashtable continuation_table;

	thread_frame_t():thread(0),
					 last_longest_child_span(0),
					 continuation_span(0),
					 prefix_span(0), 
					 lock_span(0), 
					 longest_child_span(0),
					 longest_child_lock_span(0), 
					 prefix_table(CallSiteHashtable()), 
					 longest_child_table(CallSiteHashtable()),
					 continuation_table(CallSiteHashtable()) {}
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
		void init_frame(int thread_index, int head_function_index, TRD_ID thread);

		/**
		*    @fn push()
		*    @brief Pushes a new thread_frame_t onto the thread stack 
					vector.
		*/
		std::shared_ptr<thread_frame_t> push(int head_function_index,
											 TRD_ID thread_id);

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
