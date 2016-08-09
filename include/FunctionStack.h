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
	*    @var local_work
	*    @brief Local work of the function.
	*/
	TIME local_work;

	/**
	*    @var local_lock_span
	*    @brief The lock span of the function.
	*/
	TIME lock_span;

	/**
	*    @var running_work
	*    @brief The running work of the function's call site. Same as the running
				span of the function. 
	*/
	TIME running_work;

	function_frame_t() : call_site(0), local_work(0), lock_span(0), 
						 running_work(0)
	{}
};



class FunctionStack {
	public:
		
		FunctionStack();
		~FunctionStack();

		/**
		*    @fn init_frame()
		*    @brief Initializes a function frame with index function_index 
					in the function stack. 
		*/
		void init_frame(int function_index, 
					   FUN_SG funSg,
					   CALLSITE callsiteID);

		/**
		*    @fn push()
		*    @brief Pushes a new function_frame_t onto the function stack 
					vector.
		*/
		std::shared_ptr<function_frame_t> push(FUN_SG funSg, 
											   CALLSITE callsiteID);

		/**
		*    @fn pop()
		*    @brief "Pops" a function_frame_t off the function stack 
					vector - keeps frame on stack, but decrements index
		*/
		void pop();

 		/**
		*    @var function_stack
		*    @brief Function stack that tracks bottom function in simulator,
				    implemented as a vector of std::shared_ptr<function_frame_t>.
		*/
  		std::vector< std::shared_ptr<function_frame_t> > stack;

 	private:

  		FunctionStack(const FunctionStack&);
		FunctionStack& operator=(const FunctionStack&);
};


#endif  // FunctionStack_H_ 
