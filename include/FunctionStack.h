/**
 *
 *    @file  FunctionStack.h
 *   @brief  Declarations for the function and thread stack frame structs, and 
 			 the FunctionStack class.
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef FunctionStack_H_
#define FunctionStack_H_

#include <vector> 
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



class FunctionStack {
	public:
		
		FunctionStack();
		explicit FunctionStack(std::vector< std::shared_ptr<function_frame_t> > function_stack);
		~FunctionStack();

		/**
		*    @fn init_function_frame()
		*    @brief Initializes a function frame with index function_index 
					in the function stack. 
		*/
		void init_function_frame(int function_index);

		/**
		*    @fn function_stack_push()
		*    @brief Pushes a new function_frame_t onto the function stack 
					vector.
		*/
		void function_stack_push();

		/**
		*    @fn function_stack_pop()
		*    @brief "Pops" a function_frame_t off the function stack 
					vector - keeps frame on stack, but decrements index
		*/
		void function_stack_pop();

  		/**
		*    @var current_function_index
		*    @brief The index of the simulator's current function in the
					function stack vector. 
		*/
  		int current_function_index;

 	private:

 		/**
		*    @var function_stack
		*    @brief Function stack that tracks current function in simulator,
				    implemented as a vector of std::shared_ptr<function_frame_t>.
		*/
  		std::vector< std::shared_ptr<function_frame_t> > stack;


  		FunctionStack(const FunctionStack&);
		FunctionStack& operator=(const FunctionStack&);
};


#endif  // FunctionStack_H_ 
