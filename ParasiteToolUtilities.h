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

typdef struct strand_ruler_t {

  TIME start;
  TIME stop;
} strand_ruler_t;

const char *FunctionType_str[FUNCTIONTYPE_END] = 
{ "empty", "recursive", "main", "INVALID", "cilk", "INVALID", "helper", "INVALID", "c" };



#endif
