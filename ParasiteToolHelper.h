#ifndef _PARASITE_TOOL_HELPER_H_
#define _PARASITE_TOOL_HELPER_H_

#include "ParasiteStacks.h"

void print_parallelism_data(parasite_stack_t* main_stack);

void destroy_stack(parasite_stack_t* main_stack);

void thread_end_operations(parasite_stack_t* main_stack, TIME thread_end_time);

void return_of_called_operations(parasite_stack_t* main_stack, TIME return_time);

void lock_release_operations(parasite_stack_t* main_stack);

void lock_acquire_operations(parasite_stack_t* main_stack);

void lock_release_operations(parasite_stack_t* main_stack);

void call_operations(parasite_stack_t* main_stack, unsigned long long call_site_index, TIME call_time);

void join_operations(parasite_stack_t* main_stack, TIME join_time);

void memory_access_operations(parasite_stack_t* main_stack);

void create_thread_operations(parasite_stack_t* main_stack, TIME create_time);

#endif
