#ifndef _PARASITE_TOOL_HELPER_H_
#define _PARASITE_TOOL_HELPER_H_

#include "ParasiteStacks.h"

void print_parallelism_data(parasite_stack_t* main_stack);

void destroy_stack(parasite_stack_t* main_stack);

void thread_end_operations(parasite_stack_t* main_stack);

void return_of_called_operations(parasite_stack_t* main_stack);

void lock_release_operations(parasite_stack_t* main_stack);

void lock_acquire_operations(parasite_stack_t* main_stack);

void lock_release_operations(parasite_stack_t* main_stack);

void call_operations(parasite_stack_t* main_stack);

void join_operations(parasite_stack_t* main_stack);

void memory_access_operations(parasite_stack_t* main_stack);

void create_thread_operations(parasite_stack_t* main_stack);

#endif
