#ifndef _PARASITE_TOOL_UTILITIES_H_
#define _PARASITE_TOOL_UTILITIES_H_

// Auxiliary functions for Parasite Tool

#include "ParasiteStacks.h"
#include <limits.h>
#include <string.h>
#include <time.h>

#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include <float.h>
#include <unistd.h>
#include <sys/types.h>

#include <inttypes.h>
#include <execinfo.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

// Types of functions
typedef enum {
  EMPTY = 0,
  IS_RECURSIVE = 1,
  MAIN = 2,
  SPAWNER = 4,
  HELPER = 6,
  C_FUNCTION = 8,
  FUNCTIONTYPE_END
} FunctionType_t;

const char *FunctionType_str[FUNCTIONTYPE_END] = 
{ "empty", "recursive", "main", "INVALID", "cilk", "INVALID", "helper", "INVALID", "c" };

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

#endif
