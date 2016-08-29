/**
 *
 *    @file  ParasiteTracker.h
 *   @brief  Declarations for the function and thread stack frame structs, and 
 			 the ParasiteTracker class.
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef ParasiteTracker_H_
#define ParasiteTracker_H_

#include <vector> 

#include "FunctionStack.h"
#include "ThreadStack.h"
#include "Types.h"
#include "Work.h"

class ParasiteTracker {

	public:
		
		ParasiteTracker();
		~ParasiteTracker();

		/**
		*    @fn bottomThread()
		*    @brief Gets a shared pointer for the thread at the top of the stack
		*/
		std::shared_ptr<thread_frame_t> bottomThread();

		/**
		*    @fn bottomParentThread()
		*    @brief Gets a shared pointer for the parent of the thread at the top of the stack
		*/
		std::shared_ptr<thread_frame_t> bottomParentThread();

		/**
		*    @fn bottomFunction()
		*    @brief Gets a shared pointer for the bottom function at the top of the stack
		*/
		std::shared_ptr<function_frame_t> bottomFunction();


		/**
		*    @fn bottomParentFunction()
		*    @brief Gets a shared pointer for the parent of the function at the top of the tack
		*/
		std::shared_ptr<function_frame_t> bottomParentFunction();


		/**
		*    @fn threadAt(int index)
		*    @brief Gets a shared pointer for the thread at index in the stack
		*/
		std::shared_ptr<thread_frame_t> threadAt(int index);

		/**
		*    @fn functionAt(int index)
		*    @brief Gets a shared pointer for the function at index in the stack
		*/
		std::shared_ptr<function_frame_t> functionAt(int index);

		/**
		*    @fn bottomFunctionIndex();
		*    @brief returns index of bottom function
		*/
		int bottomFunctionIndex();

		/**
		*    @fn bottomFunctionIndex();
		*    @brief returns index of bottom thread
		*/
		int bottomThreadIndex();

		std::shared_ptr<function_frame_t> function_push(FUN_SG funSg,
							   							CALLSITE callsiteID);
		void function_pop();

		std::shared_ptr<thread_frame_t> thread_push(int head_function_index,
													TRD_ID thread_id,
													vertex_descr_type first_vertex);
		void thread_pop();

 	private:

 		/**
		*    @var function_stack
		*    @brief Function stack that tracks bottom function in simulator,
				    implemented as a vector of std::shared_ptr<function_frame_t>.
		*/
		std::unique_ptr<FunctionStack> function_stack;

  		/**
		*    @var thread_stack
		*    @brief Thread stack that tracks bottom function in simulator,
				    implemented as a vector of std::shared_ptr<thread_frame_t>.
		*/
  		std::unique_ptr<ThreadStack> thread_stack;

  		ParasiteTracker(const ParasiteTracker&);
		ParasiteTracker& operator=(const ParasiteTracker&);
};


#endif  // ParasiteTracker_H_ 
