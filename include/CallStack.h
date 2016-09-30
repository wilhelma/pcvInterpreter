/**
 *
 *    @file  CallStack.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_STACK_H_
#define  CALL_STACK_H_

#include "Types.h"

#include <iostream>
#include <vector>
#include <utility>

enum class StackAction: bool {
	PUSH = 0,
	POP  = 1
};

/// Simple stack wrapping a `std::vector`
class CallStack {
  public:
		/// Pop a value from the top of the stack
    const CAL_ID pop() {
			if(isEmpty()) {
				std::cerr << "CallStack Empty!!" << std::endl;
				std::abort();
			}

      const CAL_ID callId = Stack_.back();
			Stack_.pop_back();
      return callId;
		}

		/// Push a value on top of the stack (copy it)
    void push(CAL_ID id) {
      Stack_.push_back(id);
    }

//		/// Push a value on top of the stack (move it)
//		void push(const CAL_ID&& id) { Stack_.push_back(id); }

		/// Check whether the stack is empty
		const bool isEmpty() const { return Stack_.empty(); }

		/// Top of the stack
		/// @return The value at the top of the stack without popping it
    const CAL_ID& top() const { return Stack_.back(); }

		
	private:
    std::vector<CAL_ID> Stack_;
};

#endif
