/**
 *
 *    @file  ParasiteStack.h
 *   @brief  Declarations for the function and thread stack frame structs, and 
 			 the ParasiteStack class.
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef PARASITESTACK_H_
#define PARASITESTACK_H_

#include <vector> 

#include "CallSiteHashtable.h"
#include "Types.h"

/**
*    @struct function_frame_t
*    @brief Function stack frame providing information about one function, updated with events.
*/
struct function_frame_t {


	/**
	*    @var call_site
	*    @brief The call site of the frame's function.
	*/
	CALLSITE call_site;

	/**
	*    @var function_signature
	*    @brief The signature of the function.
	*/
	FUN_SG function_signature;

	/**
	*    @var is_top_call_site_function
	*    @brief TRUE if the function is the top function of its call site.
	*/
	bool is_top_call_site_function;


	/**
	*    @var local_work
	*    @brief Local work of the function.
	*/
	double local_work;

	/**
	*    @var local_lock_span
	*    @brief The local lock span of the function.
	*/
	double local_lock_span;

	/**
	*    @var running_work
	*    @brief The running work of the function's call site. 
	*/
	double running_work;

	/**
	*    @var running_work
	*    @brief The running lock span of the function's call site. 
	*/
	double running_lock_span;

	/**
	*    @var running_span
	*    @brief The running span of the function's call site. 
	*/
	double running_span;
};

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
	*    @var local_continuation
	*    @brief Local continuation span of this thread's head function.
	*/
	double local_continuation;

	/**
	*    @var local_span
	*    @brief Local span of this thread's head function.
	*/
	double local_span;

	/**
	*    @var local_span
	*    @brief Local lock span of this thread's head function.
	*/
	double local_lock_span;

	/**
	*    @var prefix_span
	*    @brief Prefix span of this thread up to its current child function.
	*/
	double prefix_span;


	/**
	*    @var lock_span
	*    @brief Lock span of this thread up to its current child function.
	*/
	double lock_span;


	/**
	*    @var lock_span
	*    @brief Span of the longest spawned child of this thread's head function
				observed so far
	*/
	double longest_child_span;

	/**
	*    @var longest_child_lock_span
	*    @brief Lock span of the longest spawned child of this thread's head
			    function observed so far.
	*/
	double longest_child_lock_span;


	/**
	*    @var longest_child_lock_span
	*    @brief Prefix data for each call site in this thread.
	*/
	std::shared_ptr<call_site_hashtable_t> prefix_table;

	/**
	*    @var longest_child_lock_span
	*    @brief Longest child data for each call site in this thread. 
	*/
	std::shared_ptr<call_site_hashtable_t> longest_child_table;

	/**
	*    @var longest_child_lock_span
	*    @brief Continuation data for each call site in this thread.
	*/
	std::shared_ptr<call_site_hashtable_t> continuation_table;

	thread_frame_t() {

		prefix_table = std::shared_ptr<call_site_hashtable_t>(new call_site_hashtable_t);
		longest_child_table = std::shared_ptr<call_site_hashtable_t>(new call_site_hashtable_t);
		continuation_table = std::shared_ptr<call_site_hashtable_t>(new call_site_hashtable_t);
	}
};

class ParasiteStack {
	public:
		
		ParasiteStack();
		~ParasiteStack();

		/**
		*    @fn init_function_frame()
		*    @brief Initializes a function frame with index function_index 
					in the function stack. 
		*/
		void init_function_frame(int function_index);

		/**
		*    @fn init_thread_frame()
		*    @brief Initializes a thread frame with index thread_index in the 
					thread stack, and head function index head_function_index.
		*/
		void init_thread_frame(int thread_index, int head_function_index);

		/**
		*    @fn function_stack_push()
		*    @brief Pushes a new function_frame_t onto the function stack 
					vector.
		*/
		void function_stack_push();

		/**
		*    @fn thread_stack_push()
		*    @brief Pushes a new thread_frame_t onto the thread stack 
					vector.
		*/
		void thread_stack_push();


		/**
		*    @fn function_stack_pop()
		*    @brief "Pops" a function_frame_t off the function stack 
					vector - keeps frame on stack, but decrements index
		*/
		void function_stack_pop();

		/**
		*    @fn thread_stack_pop()
		*    @brief Pops a thread_frame_t off of the thread stack 
					vector.
		*/
		void thread_stack_pop();
		
		/**
		*    @var function_stack
		*    @brief Function stack that tracks current function in simulator,
				    implemented as a vector of std::shared_ptr<function_frame_t>.
		*/
  		std::vector< std::shared_ptr<function_frame_t> > function_stack;

  		/**
		*    @var thread_stack
		*    @brief Thread stack that tracks current function in simulator,
				    implemented as a vector of std::shared_ptr<thread_frame_t>.
		*/
  		std::vector< std::shared_ptr<thread_frame_t> > thread_stack;

  		/**
		*    @var work_table
		*    @brief Shared pointer to a call_site_hashtable_t which tracks the work
					for each call site. 
		*/
  		std::shared_ptr<call_site_hashtable_t> work_table;


  		/**
		*    @var current_function_index
		*    @brief The index of the simulator's current function in the
					function stack vector. 
		*/
  		int current_function_index;

  		/**
		*    @var current_thread
		*    @brief The index of the simulator's current thread in the
					thread stack vector. 
		*/
  		int current_thread_index;

 	private:
  		ParasiteStack(const ParasiteStack&);
		ParasiteStack& operator=(const ParasiteStack&);
};


#endif  // PARASITESTACK_H_ 
