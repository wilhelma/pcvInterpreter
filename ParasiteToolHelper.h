#ifndef _PARASITE_TOOL_HELPER_H_
#define _PARASITE_TOOL_HELPER_H_

#include "ParasiteStacks.h"

void print_parallelism_data(parasite_stack_t* main_stack);

void destroy_stack(parasite_stack_t* main_stack);

void thread_end_operations(parasite_stack_t* main_stack, TIME thread_end_time, TIME last_strand_start);

void return_of_called_operations(parasite_stack_t* main_stack, TIME return_time, TIME last_strand_start);

void lock_release_operations(parasite_stack_t* main_stack, double lock_span);

void lock_acquire_operations(parasite_stack_t* main_stack);

void lock_release_operations(parasite_stack_t* main_stack);

void call_operations(parasite_stack_t* main_stack, CALLSITE call_site_index, TIME call_time, TIME last_strand_start,
                     int min_capacity);

void join_operations(parasite_stack_t* main_stack, TIME last_strand_start, TIME join_time, int min_capacity);

void memory_access_operations(parasite_stack_t* main_stack);

void create_thread_operations(parasite_stack_t* main_stack, TIME last_strand_start, TIME create_time);

#endif
