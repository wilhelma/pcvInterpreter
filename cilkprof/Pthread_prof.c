#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include <float.h>
#include <unistd.h>
#include <sys/types.h>

#include <cilktool.h>

#include "Pthread_stack.h"
#include "iaddrs.h"
#include "util.h"

#ifndef SERIAL_TOOL
#define SERIAL_TOOL 1
#endif

#ifndef OLD_PRINTOUT 
#define OLD_PRINTOUT 0
#endif

#ifndef BURDENING
#define BURDENING 0
#endif

#if SERIAL_TOOL
#define GET_STACK(ex) ex
#else
#define GET_STACK(ex) REDUCER_VIEW(ex)
#include "pthreadprof_stack_reducer.h"
#endif

#include <libgen.h>

/*************************************************************************/
/**
 * Data structures for tracking work and span.
 */

#if SERIAL_TOOL
static pthreadprof_stack_t ctx_stack;
#else
static CILK_C_DECLARE_REDUCER(pthreadprof_stack_t) ctx_stack =
  CILK_C_INIT_REDUCER(pthreadprof_stack_t,
		      reduce_pthreadprof_stack,
		      identity_pthreadprof_stack,
		      destroy_pthreadprof_stack,
		      {NULL});
#endif

iaddr_table_t *call_site_table;
static iaddr_table_t *function_table;

static bool TOOL_INITIALIZED = false;
static bool TOOL_PRINTED = false;
static int TOOL_PRINT_NUM = 0;

extern int MIN_CAPACITY;
/* extern int MIN_LG_CAPACITY; */

/*************************************************************************/
/**
 * Helper methods.
 */

static inline void initialize_tool(pthreadprof_stack_t *stack) {
#if SERIAL_TOOL
  // This is a serial tool
  ensure_serial_tool();
#else
  // probably need to register the reducer here as well.
  CILK_C_REGISTER_REDUCER(ctx_stack);
#endif
  pthreadprof_stack_init(stack, MAIN);
  call_site_table = iaddr_table_create();
  function_table = iaddr_table_create();
  TOOL_INITIALIZED = true;
  TOOL_PRINTED = false;
}


/*************************************************************************/

void pthread_tool_init(void) {
  // Do the initialization only if it hasn't been done. 
  // It could have been done if we instrument C functions, and the user 
  // separately calls pthread_tool_init in the main function.

  if(!TOOL_INITIALIZED) {
    WHEN_TRACE_CALLS( fprintf(stderr, "pthread_tool_init() [ret %p]\n",
                              __builtin_extract_return_addr(__builtin_return_address(0))); );

    initialize_tool(&GET_STACK(ctx_stack));

    GET_STACK(ctx_stack).in_user_code = true;

    begin_strand(&(GET_STACK(ctx_stack)));
  }
}

/* Cleaningup; note that these cleanup may not be performed if
 * the user did not include pthread_tool_destroy in its main function and the
 * program is not compiled with -fpthreadtool_instr_c.
 */
void pthread_tool_destroy(void) {
  // Do the destroy only if it hasn't been done. 
  // It could have been done if we instrument C functions, and the user 
  // separately calls pthread_tool_destroy in the main function.
  if(TOOL_INITIALIZED) {
    WHEN_TRACE_CALLS( fprintf(stderr, "pthread_tool_destroy() [ret %p]\n",
                              __builtin_extract_return_addr(__builtin_return_address(0))); );

    pthreadprof_stack_t *stack = &GET_STACK(ctx_stack);
    // Print the output, if we haven't done so already
    if (!TOOL_PRINTED)
      pthread_tool_print();

    /* pthreadprof_stack_frame_t *old_bottom = pthreadprof_stack_pop(stack); */
    pthreadprof_stack_frame_t *old_bottom = stack->bot;
    stack->bot = NULL;

    assert(old_bottom && MAIN == old_bottom->func_type);

#if !SERIAL_TOOL
    CILK_C_UNREGISTER_REDUCER(ctx_stack);
#endif

    free_cc_hashtable(stack->wrk_table);

    old_bottom->parent = stack->sf_free_list;
    stack->sf_free_list = old_bottom;

    pthreadprof_stack_frame_t *free_frame = stack->sf_free_list;
    pthreadprof_stack_frame_t *next_free_frame;
    while (NULL != free_frame) {
      next_free_frame = free_frame->parent;
      free_cc_hashtable(free_frame->prefix_table);
      free_cc_hashtable(free_frame->lchild_table);
      free_cc_hashtable(free_frame->contin_table);
      free(free_frame);
      free_frame = next_free_frame;
    }
    stack->sf_free_list = NULL;

    free(stack->cs_status);
    free(stack->fn_status);
    free(stack->c_stack);

    cc_hashtable_list_el_t *free_list_el = ll_free_list;
    cc_hashtable_list_el_t *next_free_list_el;
    while (NULL != free_list_el) {
      next_free_list_el = free_list_el->next;
      free(free_list_el);
      free_list_el = next_free_list_el;
    }
    ll_free_list = NULL;

    // Free the tables of call sites and functions
    iaddr_table_free(call_site_table);
    call_site_table = NULL;
    iaddr_table_free(function_table);
    function_table = NULL;
    TOOL_INITIALIZED = false;
  }
}


void pthread_tool_print(void) {
  FILE *fout;
  char filename[64];

  WHEN_TRACE_CALLS( fprintf(stderr, "pthread_tool_print()\n"); );

  assert(TOOL_INITIALIZED);

  pthreadprof_stack_t *stack;
  stack = &GET_STACK(ctx_stack);

  assert(NULL != stack->bot);
  assert(MAIN == stack->bot->func_type);
  assert(stack->bot->c_head == stack->c_tail);

  pthreadprof_stack_frame_t *bottom = stack->bot;
  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  /* uint64_t span = stack->bot->prefix_spn + stack->bot->c_fn_frame->running_spn; */
  uint64_t span = bottom->prefix_spn + c_bottom->running_spn
      + bottom->local_spn + bottom->local_contin;

  add_cc_hashtables(&(bottom->prefix_table), &(bottom->contin_table));
  clear_cc_hashtable(bottom->contin_table);

  flush_cc_hashtable(&(bottom->prefix_table));

  cc_hashtable_t* span_table = bottom->prefix_table;
  fprintf(stderr, 
          "span_table->list_size = %d, span_table->table_size = %d, span_table->lg_capacity = %d\n",
  	  span_table->list_size, span_table->table_size, span_table->lg_capacity);


  /* uint64_t work = stack->bot->c_fn_frame->running_wrk; */
  uint64_t work = c_bottom->running_wrk + c_bottom->local_wrk;
  flush_cc_hashtable(&(stack->wrk_table));
  cc_hashtable_t* work_table = stack->wrk_table;
  fprintf(stderr, 
          "work_table->list_size = %d, work_table->table_size = %d, work_table->lg_capacity = %d\n",
  	  work_table->list_size, work_table->table_size, work_table->lg_capacity);

  // Read the proc maps list
  read_proc_maps();

  // Open call site CSV
  sprintf(filename, "pthreadprof_cs_%d.csv", TOOL_PRINT_NUM);
  fout = fopen(filename, "w"); 

  // print the header for the csv file
  /* fprintf(fout, "file, line, call sites (rip), depth, "); */
  fprintf(fout, "file, line, call sites (rip), function type, ");
  fprintf(fout, "work on work, span on work, parallelism on work, count on work, ");
  fprintf(fout, "top work on work, top span on work, top parallelism on work, top count on work, ");
  fprintf(fout, "local work on work, local span on work, local parallelism on work, local count on work, ");
  fprintf(fout, "work on span, span on span, parallelism on span, count on span, ");
  fprintf(fout, "top work on span, top span on span, top parallelism on span, top count on span, ");
  fprintf(fout, "local work on span, local span on span, local parallelism on span, local count on span \n");

  // Parse tables
  int span_table_entries_read = 0;
  for (size_t i = 0; i < (1 << (call_site_table->lg_capacity)); ++i) {
    iaddr_record_t *record = call_site_table->records[i];
    /* fprintf(stderr, "\n"); */
    while (NULL != record /* && (uintptr_t)NULL != record->iaddr */) {
      /* fprintf(stderr, "rip %p (%d), ", record->iaddr, record->index); */

      assert(0 != record->iaddr);
      assert(0 <= record->index && record->index < (1 << work_table->lg_capacity));

      cc_hashtable_entry_t *entry = &(work_table->entries[ record->index ]);

      if (empty_cc_entry_p(entry)) {
        record = record->next;
        continue;
      }

      assert(entry->rip == record->iaddr);

      uint64_t wrk_wrk = entry->wrk;
      uint64_t spn_wrk = entry->spn;
      double par_wrk = (double)wrk_wrk/(double)spn_wrk;
      uint64_t cnt_wrk = entry->count;

      uint64_t t_wrk_wrk = entry->top_wrk;
      uint64_t t_spn_wrk = entry->top_spn;
      double t_par_wrk = (double)t_wrk_wrk/(double)t_spn_wrk;
      uint64_t t_cnt_wrk = entry->top_count;

      uint64_t l_wrk_wrk = entry->local_wrk;
      uint64_t l_spn_wrk = entry->local_spn;
      double l_par_wrk = (double)l_wrk_wrk/(double)l_spn_wrk;
      uint64_t l_cnt_wrk = entry->local_count;

      uint64_t wrk_spn = 0;
      uint64_t spn_spn = 0;
      double par_spn = DBL_MAX;
      uint64_t cnt_spn = 0;

      uint64_t t_wrk_spn = 0;
      uint64_t t_spn_spn = 0;
      double t_par_spn = DBL_MAX;
      uint64_t t_cnt_spn = 0;

      uint64_t l_wrk_spn = 0;
      uint64_t l_spn_spn = 0;
      double l_par_spn = DBL_MAX;
      uint64_t l_cnt_spn = 0;

      if (record->index < (1 << span_table->lg_capacity)) {
        cc_hashtable_entry_t *st_entry = &(span_table->entries[ record->index ]);

        if (!empty_cc_entry_p(st_entry)) {
          assert(st_entry->rip == entry->rip);

          wrk_spn = st_entry->wrk;
          spn_spn = st_entry->spn;
          par_spn = (double)wrk_spn / (double)spn_spn;
          cnt_spn = st_entry->count;

          t_wrk_spn = st_entry->top_wrk;
          t_spn_spn = st_entry->top_spn;
          t_par_spn = (double)t_wrk_spn / (double)t_spn_spn;
          t_cnt_spn = st_entry->top_count;

          l_wrk_spn = st_entry->local_wrk;
          l_spn_spn = st_entry->local_spn;
          l_par_spn = (double)l_wrk_spn / (double)l_spn_spn;
          l_cnt_spn = st_entry->local_count;

          ++span_table_entries_read;
        }
      }

      int line = 0; 
      char *fstr = NULL;

      uint64_t addr = rip2cc(record->iaddr);

      // get_info_on_inst_addr returns a char array from some system call that
      // needs to get freed by the user after we are done with the info
      char *line_to_free = get_info_on_inst_addr(addr, &line, &fstr);
      char *file = basename(fstr);

      fprintf(fout, "\"%s\", %d, 0x%lx, ", file, line, addr);

      FunctionType_t func_type = record->func_type;
      if (stack->cs_status[record->index].flags & RECURSIVE) {  // recursive function
        fprintf(fout, "%s %s, ",
                FunctionType_str[func_type],
                FunctionType_str[IS_RECURSIVE]);
      } else {
        fprintf(fout, "%s, ", FunctionType_str[func_type]);
      }
      fprintf(fout, "%lu, %lu, %g, %lu, %lu, %lu, %g, %lu, %lu, %lu, %g, %lu, ", 
              wrk_wrk, spn_wrk, par_wrk, cnt_wrk,
              t_wrk_wrk, t_spn_wrk, t_par_wrk, t_cnt_wrk,
              l_wrk_wrk, l_spn_wrk, l_par_wrk, l_cnt_wrk);
      fprintf(fout, "%lu, %lu, %g, %lu, %lu, %lu, %g, %lu, %lu, %lu, %g, %lu\n", 
              wrk_spn, spn_spn, par_spn, cnt_spn,
              t_wrk_spn, t_spn_spn, t_par_spn, t_cnt_spn,
              l_wrk_spn, l_spn_spn, l_par_spn, l_cnt_spn);
      if(line_to_free) free(line_to_free);
      
      record = record->next;
    }
  }
  fclose(fout);


  assert(span_table_entries_read == span_table->table_size);

  fprintf(stderr, "work = %fs, span = %fs, parallelism = %f\n",
	  work / (1000000000.0),
	  span / (1000000000.0),
	  work / (float)span);

  // Free the proc maps list
  mapping_list_el_t *map_lst_el = maps.head;
  mapping_list_el_t *next_map_lst_el;
  while (NULL != map_lst_el) {
    next_map_lst_el = map_lst_el->next;
    free(map_lst_el);
    map_lst_el = next_map_lst_el;
  }
  maps.head = NULL;
  maps.tail = NULL;

  TOOL_PRINTED = true;
  ++TOOL_PRINT_NUM;
}


/*************************************************************************/
/**
 * Hooks into runtime system.
 */

void pthread_enter_begin(void* this_fn, void* rip)
{

  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  if (!TOOL_INITIALIZED) {
    initialize_tool(&(ctx_stack));

  } else {
    stack = &(GET_STACK(ctx_stack));

    uint64_t strand_len = measure_and_add_strand_length(stack);
    if (stack->bot->c_head == stack->c_tail) {
      stack->bot->local_contin += strand_len;
    }
    stack->in_user_code = false;
  }

  // Push new frame onto the stack
  pthreadprof_stack_push(stack, SPAWNER);

  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(rip);
  uintptr_t fn = (uintptr_t)this_fn;

  int32_t cs_index = add_to_iaddr_table(&call_site_table, cs, SPAWNER);
  c_bottom->cs_index = cs_index;
  if (cs_index >= stack->cs_status_capacity) {
    resize_cs_status_vector(&(stack->cs_status), &(stack->cs_status_capacity));
  }
  int32_t cs_tail = stack->cs_status[cs_index].c_tail;
  if (OFF_STACK != cs_tail) {
    if (!(stack->cs_status[cs_index].flags & RECURSIVE)) {
      stack->cs_status[cs_index].flags |= RECURSIVE;
    }
  } else {
    int32_t fn_index;
    if (UNINITIALIZED == stack->cs_status[cs_index].fn_index) {

      assert(call_site_table->table_size == cs_index + 1);
      MIN_CAPACITY = cs_index + 1;

      fn_index = add_to_iaddr_table(&function_table, fn, SPAWNER);
      stack->cs_status[cs_index].fn_index = fn_index;
      if (fn_index >= stack->fn_status_capacity) {
        resize_fn_status_vector(&(stack->fn_status), &(stack->fn_status_capacity));
      }
    } else {
      fn_index = stack->cs_status[cs_index].fn_index;
    }
    stack->cs_status[cs_index].c_tail = stack->c_tail;
    if (OFF_STACK == stack->fn_status[fn_index]) {
      stack->fn_status[fn_index] = stack->c_tail;
    }
  }

  c_bottom->rip = (uintptr_t)__builtin_extract_return_addr(rip);
  c_bottom->function = (uintptr_t)this_fn;
}


void pthread_enter_helper_begin(void *this_fn, void *rip)
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  // We should have passed spawn_or_continue(0) to get here.
  assert(stack->in_user_code);

  // A helper should not be invoked from a C function
  assert(stack->bot->c_head == stack->c_tail);

  uint64_t strand_len = measure_and_add_strand_length(stack);
  stack->bot->local_contin += strand_len;

  stack->in_user_code = false;

  // Push new frame onto the stack
  pthreadprof_stack_push(stack, HELPER);

  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(rip);
  uintptr_t fn = (uintptr_t)this_fn;

  int32_t cs_index = add_to_iaddr_table(&call_site_table, cs, HELPER);
  c_bottom->cs_index = cs_index;
  if (cs_index >= stack->cs_status_capacity) {
    resize_cs_status_vector(&(stack->cs_status), &(stack->cs_status_capacity));
  }
  int32_t cs_tail = stack->cs_status[cs_index].c_tail;
  if (OFF_STACK != cs_tail) {
    if (!(stack->cs_status[cs_index].flags & RECURSIVE)) {
      stack->cs_status[cs_index].flags |= RECURSIVE;
    }
  } else {
    int32_t fn_index;
    if (UNINITIALIZED == stack->cs_status[cs_index].fn_index) {

      assert(call_site_table->table_size == cs_index + 1);
      MIN_CAPACITY = cs_index + 1;

      fn_index = add_to_iaddr_table(&function_table, fn, SPAWNER);
      stack->cs_status[cs_index].fn_index = fn_index;
      if (fn_index >= stack->fn_status_capacity) {
        resize_fn_status_vector(&(stack->fn_status), &(stack->fn_status_capacity));
      }
    } else {
      fn_index = stack->cs_status[cs_index].fn_index;
    }
    stack->cs_status[cs_index].c_tail = stack->c_tail;
    if (OFF_STACK == stack->fn_status[fn_index]) {
      stack->fn_status[fn_index] = stack->c_tail;
    }
  }
}

void pthread_enter_end()
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  assert(!(stack->in_user_code));

  stack->in_user_code = true;

  begin_strand(stack);
}

void pthread_tool_c_function_enter(void *this_fn, void *rip)
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  if(!TOOL_INITIALIZED) { // We are entering main.
    pthread_tool_init(); // this will push the frame for MAIN and do a gettime

    c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

    uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(rip);
    uintptr_t fn = (uintptr_t)this_fn;

    int32_t cs_index = add_to_iaddr_table(&call_site_table, cs, MAIN);
    c_bottom->cs_index = cs_index;
    if (cs_index >= stack->cs_status_capacity) {
      resize_cs_status_vector(&(stack->cs_status), &(stack->cs_status_capacity));
    }
    stack->cs_status[cs_index].c_tail = stack->c_tail;
    assert(call_site_table->table_size == cs_index + 1);
    MIN_CAPACITY = cs_index + 1;

    int32_t fn_index = add_to_iaddr_table(&function_table, fn, MAIN);
    stack->cs_status[cs_index].fn_index = fn_index;
    /* c_bottom->fn_index = fn_index; */
    if (fn_index >= stack->fn_status_capacity) {
      resize_fn_status_vector(&(stack->fn_status), &(stack->fn_status_capacity));
    }
    assert(OFF_STACK == stack->fn_status[fn_index]);
    stack->fn_status[fn_index] = stack->c_tail;

  } else {
    if (!stack->in_user_code) {
      WHEN_TRACE_CALLS( fprintf(stderr, "c_function_enter(%p) [ret %p]\n", rip,
                                __builtin_extract_return_addr(__builtin_return_address(0))); );
    }
    assert(stack->in_user_code);

    uint64_t strand_len = measure_and_add_strand_length(stack);
    if (stack->bot->c_head == stack->c_tail) {
      stack->bot->local_contin += strand_len;
    }

    // Push new frame for this C function onto the stack
    c_fn_frame_t *c_bottom = pthreadprof_c_fn_push(stack);

    uintptr_t cs = (uintptr_t)__builtin_extract_return_addr(rip);
    uintptr_t fn = (uintptr_t)this_fn;

    int32_t cs_index = add_to_iaddr_table(&call_site_table, cs, C_FUNCTION);
    c_bottom->cs_index = cs_index;
    if (cs_index >= stack->cs_status_capacity) {
      resize_cs_status_vector(&(stack->cs_status), &(stack->cs_status_capacity));
    }
    int32_t cs_tail = stack->cs_status[cs_index].c_tail;
    if (OFF_STACK != cs_tail) {
      if (!(stack->cs_status[cs_index].flags & RECURSIVE)) {
        stack->cs_status[cs_index].flags |= RECURSIVE;
      }
    } else {
      int32_t fn_index;
      if (UNINITIALIZED == stack->cs_status[cs_index].fn_index) {

        assert(call_site_table->table_size == cs_index + 1);
        MIN_CAPACITY = cs_index + 1;

        fn_index = add_to_iaddr_table(&function_table, fn, C_FUNCTION);
        stack->cs_status[cs_index].fn_index = fn_index;
        if (fn_index >= stack->fn_status_capacity) {
          resize_fn_status_vector(&(stack->fn_status), &(stack->fn_status_capacity));
        }
      } else {
        fn_index = stack->cs_status[cs_index].fn_index;
      }
      stack->cs_status[cs_index].c_tail = stack->c_tail;
      if (OFF_STACK == stack->fn_status[fn_index]) {
        stack->fn_status[fn_index] = stack->c_tail;
      }
    }

    c_bottom->rip = (uintptr_t)__builtin_extract_return_addr(rip);
    c_bottom->function = (uintptr_t)this_fn;

    /* the stop time is also the start time of this function */
    // stack->start = stack->stop; /* TB: Want to exclude the length
    // (e.g. time or instruction count) of this function */

    begin_strand(stack);
  }
}

void pthread_tool_c_function_leave(void *rip)
{

  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  const c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);
  if (NULL != stack->bot &&
      MAIN == stack->bot->func_type &&
      stack->c_tail == stack->bot->c_head) {

    int32_t cs_index = c_bottom->cs_index;
    int32_t cs_tail = stack->cs_status[cs_index].c_tail;
    bool top_cs = (cs_tail == stack->c_tail);

    if (top_cs) {
      stack->cs_status[cs_index].c_tail = OFF_STACK;
      int32_t fn_index = stack->cs_status[cs_index].fn_index;
      if (stack->fn_status[fn_index] == stack->c_tail) {
        stack->fn_status[fn_index] = OFF_STACK;
      }
    }

    pthread_tool_destroy();
  }
  if (!TOOL_INITIALIZED) {
    // either user code already called pthread_tool_destroy, or we are leaving
    // main; in either case, nothing to do here;
    return;
  }

  bool add_success;
  /* pthreadprof_stack_frame_t *old_bottom; */
  const c_fn_frame_t *old_bottom;

  assert(stack->in_user_code);
  // stop the timer and attribute the elapsed time to this returning
  // function

  measure_and_add_strand_length(stack);

  assert(stack->c_tail > stack->bot->c_head);

  // Pop the stack
  old_bottom = pthreadprof_c_fn_pop(stack);
  /* assert(old_bottom->local_wrk == old_bottom->local_contin); */
  uint64_t local_wrk = old_bottom->local_wrk;
  uint64_t running_wrk = old_bottom->running_wrk + local_wrk;
  uint64_t running_spn = old_bottom->running_spn + local_wrk;

  int32_t cs_index = old_bottom->cs_index;
  int32_t cs_tail = stack->cs_status[cs_index].c_tail;
  bool top_cs = (cs_tail == stack->c_tail + 1);

  if (top_cs) {  // top CS instance
    stack->cs_status[cs_index].c_tail = OFF_STACK;
    int32_t fn_index = stack->cs_status[cs_index].fn_index;
    if (stack->fn_status[fn_index] == stack->c_tail + 1) {
      stack->fn_status[fn_index] = OFF_STACK;
    }
  }

  c_fn_frame_t *new_bottom = &(stack->c_stack[stack->c_tail]);
  new_bottom->running_wrk += running_wrk;
  new_bottom->running_spn += running_spn;

  // Update work table
  if (top_cs) {
    uint32_t fn_index = stack->cs_status[new_bottom->cs_index].fn_index;
    /* fprintf(stderr, "adding to wrk table\n"); */
    add_success = add_to_cc_hashtable(&(stack->wrk_table),
                                      stack->c_tail == stack->fn_status[fn_index],
                                      cs_index,
                                      old_bottom->rip,
                                      running_wrk,
                                      running_spn,
                                      local_wrk,
                                      local_wrk);
    assert(add_success);
    /* fprintf(stderr, "adding to prefix table\n"); */
    add_success = add_to_cc_hashtable(&(stack->bot->contin_table),
                                      stack->c_tail == stack->fn_status[fn_index],
                                      cs_index,
                                      old_bottom->rip,
                                      running_wrk,
                                      running_spn,
                                      local_wrk,
                                      local_wrk);
    assert(add_success);
  } else {
    // Only record the local work and local span
    /* fprintf(stderr, "adding to wrk table\n"); */
    add_success = add_local_to_cc_hashtable(&(stack->wrk_table),
                                            cs_index,
                                            old_bottom->rip,
                                            local_wrk,
                                            local_wrk);
    assert(add_success);
    /* fprintf(stderr, "adding to contin table\n"); */
    add_success = add_local_to_cc_hashtable(&(stack->bot->contin_table),
                                            cs_index,
                                            old_bottom->rip,
                                            local_wrk,
                                            local_wrk);
    assert(add_success);
  }

  stack->strand_start
      = (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));
  begin_strand(stack);
}

void pthread_create_prepare()
{
  // Tool must have been initialized as this is only called in a SPAWNER, and 
  // we would have at least initialized the tool in the first pthread_enter_begin.
  assert(TOOL_INITIALIZED);

  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  stack->strand_end
      = (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));

  uint64_t strand_len = measure_and_add_strand_length(stack);
  stack->bot->local_contin += strand_len;

  assert(stack->c_tail == stack->bot->c_head);

  assert(stack->in_user_code);
  stack->in_user_code = false;
}

// If in_continuation == 0, we just did setjmp and about to call the spawn helper.  
// If in_continuation == 1, we are resuming after setjmp (via longjmp) at the continuation 
// of a spawn statement; note that this is possible only if steals can occur.
void pthread_create_or_continue(int in_continuation)
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  assert(stack->c_tail == stack->bot->c_head);

  assert(!(stack->in_user_code));
  if (in_continuation) {

    // In the continuation
    stack->in_user_code = true;

    stack->bot->local_contin += BURDENING;

    stack->strand_start
      = (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));

    begin_strand(stack);
  } else {

    // In the spawned child
    // We need to re-enter user code, because function calls for
    // arguments might be called before enter_helper_begin occurs in
    // spawn helper.
    stack->in_user_code = true;

    stack->strand_start
      = (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));
    begin_strand(stack);
  }
}

void pthread_detach_begin()
{

  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));
  assert(HELPER == stack->bot->func_type);

  uint64_t strand_len = measure_and_add_strand_length(stack);
  stack->bot->local_contin += strand_len;

  assert(stack->bot->c_head == stack->c_tail);

  assert(stack->in_user_code);
  stack->in_user_code = false;

  return;
}

void pthread_detach_end(void)
{

  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  assert(stack->bot->c_head == stack->c_tail);
  
  assert(!(stack->in_user_code));
  stack->in_user_code = true;

  begin_strand(stack);

  return;
}

void pthread_sync_begin()
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  uint64_t strand_len = measure_and_add_strand_length(stack);
  stack->bot->local_contin += strand_len;

  assert(stack->bot->c_head == stack->c_tail);

  assert(stack->in_user_code);
  stack->in_user_code = false;
}

void pthread_sync_end()
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  assert(stack->bot->c_head == stack->c_tail);

  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  c_bottom->running_spn += stack->bot->local_contin;

  /* if (stack->bot->lchild_spn > stack->bot->contin_spn) { */
  if (stack->bot->lchild_spn > c_bottom->running_spn) {
    stack->bot->prefix_spn += stack->bot->lchild_spn;
    // local_spn does not increase, because critical path goes through
    // spawned child.
    add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->lchild_table));

  } else {
    /* stack->bot->prefix_spn += stack->bot->contin_spn; */
    stack->bot->prefix_spn += c_bottom->running_spn;
    // critical path goes through continuation, which is local.  add
    // local_contin to local_spn.
    stack->bot->local_spn += stack->bot->local_contin;
    add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->contin_table));
  }

  // reset lchild and contin span variables
  stack->bot->lchild_spn = 0;
  c_bottom->running_spn = 0;
  stack->bot->local_contin = 0;
  clear_cc_hashtable(stack->bot->lchild_table);
  clear_cc_hashtable(stack->bot->contin_table);

  // can't be anything else; only SPAWNER have sync
  assert(SPAWNER == stack->bot->func_type); 
  assert(!(stack->in_user_code));
  stack->in_user_code = true;

  begin_strand(stack);
}

void pthread_leave_begin()
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));

  pthreadprof_stack_frame_t *old_bottom;
  bool add_success;

  uint64_t strand_len = measure_and_add_strand_length(stack);
  stack->bot->local_contin += strand_len;

  assert(stack->in_user_code);
  stack->in_user_code = false;

  // We are leaving this function, so it must have sync-ed, meaning
  // that, lchild should be 0 / empty.  prefix could contain value,
  // however, if this function is a Cilk function that spawned before.
  assert(0 == stack->bot->lchild_spn);
  assert(cc_hashtable_is_empty(stack->bot->lchild_table));
  assert(stack->bot->c_head == stack->c_tail);

  c_fn_frame_t *old_c_bottom = &(stack->c_stack[stack->c_tail]);

  stack->bot->prefix_spn += old_c_bottom->running_spn;
  stack->bot->local_spn += stack->bot->local_contin + BURDENING;
  old_c_bottom->running_wrk += old_c_bottom->local_wrk;
  stack->bot->prefix_spn += stack->bot->local_spn;

  add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->contin_table));

  // Pop the stack
  old_bottom = pthreadprof_stack_pop(stack);

  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  int32_t cs_index = old_c_bottom->cs_index;
  int32_t cs_tail = stack->cs_status[cs_index].c_tail;
  bool top_cs = (cs_tail == stack->c_tail + 1);

  if (top_cs) {  // top CS instance
    stack->cs_status[cs_index].c_tail = OFF_STACK;
    int32_t fn_index = stack->cs_status[cs_index].fn_index;
    if (stack->fn_status[fn_index] == stack->c_tail + 1) {
      stack->fn_status[fn_index] = OFF_STACK;
    }
  }

  c_bottom->running_wrk += old_c_bottom->running_wrk;

  // Update work table
  if (top_cs) {
    /* assert(stack->cs_status[c_bottom->cs_index].fn_index == c_bottom->fn_index); */
    int32_t fn_index = stack->cs_status[c_bottom->cs_index].fn_index;
    /* fprintf(stderr, "adding to wrk table\n"); */
    add_success = add_to_cc_hashtable(&(stack->wrk_table),
                                      stack->c_tail == stack->fn_status[fn_index],
                                      cs_index,
                                      old_c_bottom->rip,
                                      old_c_bottom->running_wrk,
                                      old_bottom->prefix_spn,
                                      old_c_bottom->local_wrk,
                                      old_bottom->local_spn);
    assert(add_success);
    /* fprintf(stderr, "adding to prefix table\n"); */
    add_success = add_to_cc_hashtable(&(old_bottom->prefix_table),
                                      stack->c_tail == stack->fn_status[fn_index],
                                      cs_index,
                                      old_c_bottom->rip,
                                      old_c_bottom->running_wrk,
                                      old_bottom->prefix_spn,
                                      old_c_bottom->local_wrk,
                                      old_bottom->local_spn);
    assert(add_success);
  } else {
    // Only record the local work and local span
    /* fprintf(stderr, "adding to wrk table\n"); */
    add_success = add_local_to_cc_hashtable(&(stack->wrk_table),
                                            cs_index,
                                            old_c_bottom->rip,
                                            old_c_bottom->local_wrk,
                                            old_bottom->local_spn);
    assert(add_success);
    /* fprintf(stderr, "adding to prefix table\n"); */
    add_success = add_local_to_cc_hashtable(&(old_bottom->prefix_table),
                                            cs_index,
                                            old_c_bottom->rip,
                                            old_c_bottom->local_wrk,
                                            old_bottom->local_spn);
    assert(add_success);
  }

  if (SPAWNER == old_bottom->func_type) {
    // This is the case we are returning to a call, since a SPAWNER
    // is always called by a spawn helper.

    assert(NULL != old_bottom->parent);

    // Update continuation variable
    c_bottom->running_spn += old_bottom->prefix_spn;
    // Don't increment local_spn for new stack->bot.
    /* fprintf(stderr, "adding tables\n"); */
    add_cc_hashtables(&(stack->bot->contin_table), &(old_bottom->prefix_table));

    clear_cc_hashtable(old_bottom->prefix_table);
    clear_cc_hashtable(old_bottom->lchild_table);
    clear_cc_hashtable(old_bottom->contin_table);

  } else {
    // This is the case we are returning to a spawn, since a HELPER 
    // is always invoked due to a spawn statement.

    assert(HELPER != stack->bot->func_type);

    if (c_bottom->running_spn + old_bottom->prefix_spn > stack->bot->lchild_spn) {
      // fprintf(stderr, "updating longest child\n");
      stack->bot->prefix_spn += c_bottom->running_spn;
      stack->bot->local_spn += stack->bot->local_contin;
      // This needs a better data structure to be implemented more
      // efficiently.
      add_cc_hashtables(&(stack->bot->prefix_table), &(stack->bot->contin_table));

      // Save old_bottom tables in new bottom's l_child variable.
      stack->bot->lchild_spn = old_bottom->prefix_spn;
      clear_cc_hashtable(stack->bot->lchild_table);
      /* free(stack->bot->lchild_table); */
      cc_hashtable_t* tmp_cc = stack->bot->lchild_table;
      stack->bot->lchild_table = old_bottom->prefix_table;
      old_bottom->prefix_table = tmp_cc;

      // Free old_bottom tables that are no longer in use
      clear_cc_hashtable(old_bottom->lchild_table);
      clear_cc_hashtable(old_bottom->contin_table);

      // Empy new bottom's continuation
      c_bottom->running_spn = 0;
      stack->bot->local_contin = 0;
      clear_cc_hashtable(stack->bot->contin_table);

    } else {
      // Discared all tables from old_bottom
      clear_cc_hashtable(old_bottom->prefix_table);
      clear_cc_hashtable(old_bottom->lchild_table);
      clear_cc_hashtable(old_bottom->contin_table);

    }
  }

  /* free(old_bottom); */
  old_bottom->parent = stack->sf_free_list;
  stack->sf_free_list = old_bottom;
}

void pthread_leave_end(void)
{
  pthreadprof_stack_t *stack = &(GET_STACK(ctx_stack));


  assert(!(stack->in_user_code));
  stack->in_user_code = true;
  begin_strand(stack);
}

#include "cc_hashtable.c"
#include "util.c"
#include "iaddrs.c"
