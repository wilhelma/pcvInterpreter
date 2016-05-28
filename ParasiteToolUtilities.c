//#include "ParasiteToolUtilities.h"

__attribute__((always_inline))
void begin_strand(parasite_stack_t *stack) {
  start_strand(&(stack->strand_ruler));
}

__attribute__((always_inline))
uint64_t measure_and_add_strand_length(parasite_stack_t *stack) {
  // Measure strand length

  int strand_len = measure_strand_length(&(stack->strand_ruler));
  assert(NULL != stack->bottom_parasite_frame);

  // Accumulate strand length
  stack->function_stack[stack->function_stack_tail_index].local_work += strand_len;
  return strand_len;
}