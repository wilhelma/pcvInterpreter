#ifndef _PARASITE_UTILITIES_H_
#define _PARASITE_UTILITIES_H_

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

const char *FunctionType_str[FUNCTIONTYPE_END];

// Linked list of mappings.
typedef struct mapping_t {
  uintptr_t low, high;
  char *path;
} mapping_t;

typedef struct mapping_list_el_t {
  mapping_t map;
  struct mapping_list_el_t* next;
} mapping_list_el_t;

typedef struct mapping_list_t {
  mapping_list_el_t *head;
  mapping_list_el_t *tail;
} mapping_list_t;

extern mapping_list_t maps;

// Utility functions
void ensure_serial_tool(void);
uintptr_t rip2cc(uintptr_t rip);
char* get_info_on_inst_addr(uint64_t addr, int *line_no, char **file);
void print_addr(uintptr_t a);

#endif