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
	TIME local_work;

	/**
	*    @var local_lock_span
	*    @brief The local lock span of the function.
	*/
	TIME local_lock_span;

	/**
	*    @var running_work
	*    @brief The running work of the function's call site. 
	*/
	TIME running_work;

	/**
	*    @var running_work
	*    @brief The running lock span of the function's call site. 
	*/
	TIME running_lock_span;

	/**
	*    @var running_span
	*    @brief The running span of the function's call site. 
	*/
	TIME running_span;

	function_frame_t() : call_site(0), local_work(0), local_lock_span(0),
						 running_work(0), running_lock_span(0), running_span(0)
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
					   CALLSITE callsiteID,
					   bool is_top_call_site_function);

		/**
		*    @fn push()
		*    @brief Pushes a new function_frame_t onto the function stack 
					vector.
		*/
		std::shared_ptr<function_frame_t> push(FUN_SG funSg, 
											   CALLSITE callsiteID, 
											   bool is_top_call_site_function);

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
