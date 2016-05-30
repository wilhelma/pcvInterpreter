#include "ParasiteToolHelper.h"

void memory_access_operations(parasite_stack_t* main_stack) {

  return;
}

void create_thread_operations(parasite_stack_t* main_stack, TIME create_time) {

  TIME strand_len = create_time - last_strand_start;
  last_strand_start = create_time;
  main_stack->function_stack[stack->function_stack_tail_index].local_work += strand_len;
  main_stack->bottom_parasite_frame->local_continuation += strand_len;

  assert(main_stack->function_stack_tail_index == main_stack->bottom_parasite_frame->head_function_index);
}

void join_operations(parasite_stack_t* main_stack, TIME join_time) {

  // F syncs

  // if F.l > F.c
  //    F.p += F.l
  //    F.p += F.lock_span
  //    F.p -= F.longest_child_lock_span
  // else
  //    F.p += F.c
  // F.c = 0
  // F.l = 0
  // F.longest_child_lock_span = 0

  TIME strand_len = join_time - last_strand_start;
  last_strand_start = join_time;
  main_stack->function_stack[stack->function_stack_tail_index].local_work += strand_len;
  main_stack->bottom_parasite_frame->local_continuation += strand_len;

  assert(main_stack->bottom_parasite_frame->head_function_index == main_stack->function_stack_tail_index);

  function_frame_t *bottom_function_frame= &(main_stack->function_stack[main_stack->function_stack_tail_index]);

  bottom_function_frame->running_span += main_stack->bottom_parasite_frame->local_continuation;

  // if F.l > F.c
  if (main_stack->bottom_parasite_frame->longest_child_span > bottom_function_frame->running_span) {

    // F.p += F.l
    // F.p += F.lock_span
    // F.p -= F.longest_child_lock_span
    main_stack->bottom_parasite_frame->prefix_span += main_stack->bottom_parasite_frame->longest_child_span;
    main_stack->bottom_parasite_frame->prefix_span += main_stack->bottom_parasite_frame->lock_span;
    main_stack->bottom_parasite_frame->prefix_span += main_stack->bottom_parasite_frame->longest_child_lock_span;

    // local_span does not increase, because critical path goes through
    // spawned child.
    add_parasite_hashtables(&(main_stack->bottom_parasite_frame->prefix_table), &(main_stack->bottom_parasite_frame->longest_child_table));

  } 

  else {

    // F.p += F.c
    main_stack->bottom_parasite_frame->prefix_span += bottom_function_frame->running_span;

    // critical path goes through continuation, which is local.  add
    // local_continuation to local_span.
    main_stack->bottom_parasite_frame->local_span += main_stack->bottom_parasite_frame->local_continuation;
    add_parasite_hashtables(&(main_stack->bottom_parasite_frame->prefix_table), &(main_stack->bottom_parasite_frame->continuation_table));

  }

  // reset longest child and continuation span variables

  // F.c = 0
  // F.l = 0
  // F.longest_child_lock_span = 0
  main_stack->bottom_parasite_frame->local_continuation = 0;
  main_stack->bottom_parasite_frame->longest_child_span = 0;
  main_stack->bottom_parasite_frame->longest_child_lock_span = 0;
  
  bottom_function_frame->running_span = 0;

  clear_parasite_hashtable(main_stack->bottom_parasite_frame->longest_child_table);
  clear_parasite_hashtable(main_stack->bottom_parasite_frame->continuation_table);

  // can't be anything else; only SPAWNER have sync
  assert(SPAWNER == main_stack->bottom_parasite_frame->func_type); 
}


void call_operations(parasite_stack_t* main_stack, unsigned long long call_site_index, TIME call_time) {

    // F spawns or calls G:
    // G.w = 0
    // G.p = 0
    // G.l = 0
    // G.c = 0

    TIME strand_len = call_time - last_strand_start;
    last_strand_start = call_time;
    main_stack->function_stack[stack->function_stack_tail_index].local_work += strand_len;

    if (main_stack->bottom_parasite_frame->head_function_index == main_stack->function_stack_tail_index) {

      main_stack->bottom_parasite_frame->local_continuation += strand_len;
    }

    // Push new frame for this C function onto the main_stack
    function_frame_t *bottom_function_frame= parasite_function_push(main_stack);

    bottom_function_frame->call_site_index = call_site_index;

    if (call_site_index >= main_stack->call_site_status_vector_capacity) {
      resize_call_site_status_vector(&(main_stack->call_site_status_vector), &(main_stack->call_site_status_vector_capacity));
    }
    int32_t cs_tail = main_stack->call_site_status_vector[call_site_index].call_site_tail_function_index;
    if (OFF_STACK != cs_tail) {
      if (!(main_stack->call_site_status_vector[call_site_index].flags & RECURSIVE)) {
        main_stack->call_site_status_vector[call_site_index].flags |= RECURSIVE;
      }
    } else {
      int32_t call_site_function_index;
      if (UNINITIALIZED == main_stack->call_site_status_vector[call_site_index].call_site_function_index) {

        MIN_CAPACITY = call_site_index + 1;

        main_stack->call_site_status_vector[call_site_index].call_site_function_index = call_site_function_index;
        if (call_site_function_index >= main_stack->function_status_vector_capacity) {
          resize_function_status_vector(&(main_stack->function_status_vector), &(main_stack->function_status_vector_capacity));
        }
      } else {
        call_site_function_index = main_stack->call_site_status_vector[call_site_index].call_site_function_index;
      }
      main_stack->call_site_status_vector[call_site_index].call_site_tail_function_index = main_stack->function_stack_tail_index;
      if (OFF_STACK == main_stack->function_status_vector[call_site_function_index]) {
        main_stack->function_status_vector[call_site_function_index] = main_stack->function_stack_tail_index;
      }
    }
}


void destroy_stack(parasite_stack_t* main_stack) {

	  parasite_stack_frame_t *old_bottom_parasite_frame = main_stack->bottom_parasite_frame;
    free_parasite_hashtable(main_stack->work_table);

    old_bottom_parasite_frame->parent = main_stack->stack_frame_free_list;
    main_stack->stack_frame_free_list = old_bottom_parasite_frame;

    parasite_stack_frame_t *free_frame = main_stack->stack_frame_free_list;
    parasite_stack_frame_t *next_free_frame;

    while (NULL != free_frame) {

      next_free_frame = free_frame->parent;
      free_parasite_hashtable(free_frame->prefix_table);
      free_parasite_hashtable(free_frame->longest_child_table);
      free_parasite_hashtable(free_frame->continuation_table);
      free(free_frame);
      free_frame = next_free_frame;
    }

    main_stack->stack_frame_free_list = NULL;

    free(main_stack->call_site_status_vector);
    free(main_stack->function_status_vector);
    free(main_stack->function_stack);

    parasite_hashtable_linked_list_node_t *free_list_node = linked_list_free_list;
    parasite_hashtable_linked_list_node_t *next_free_list_node;

    while (NULL != free_list_node) {
      next_free_list_node = free_list_node->next;
      free(free_list_node);
      free_list_node = next_free_list_node;
    }
    linked_list_free_list = NULL;

}

void print_parallelism_data(parasite_stack_t* main_stack) {

  FILE *fout;
  char filename[64];

  assert(NULL != main_stack->bottom_parasite_frame);
  assert(MAIN == main_stack->bottom_parasite_frame->func_type);
  assert(main_stack->bottom_parasite_frame->head_function_index == main_stack->function_stack_tail_index);

  parasite_stack_frame_t *bottom_parasite_frame = main_stack->bottom_parasite_frame;
  function_frame_t *bottom_function_frame= &(main_stack->function_stack[main_stack->function_stack_tail_index]);

  double span = bottom_parasite_frame->prefix_span + bottom_function_frame->running_span
      + bottom_parasite_frame->local_span + bottom_parasite_frame->local_continuation;

  // span = prefix + continuation
  add_parasite_hashtables(&(bottom_parasite_frame->prefix_table), &(bottom_parasite_frame->continuation_table));
  clear_parasite_hashtable(bottom_parasite_frame->continuation_table);

  // combine the entries in the hashtable for each call site
  flush_parasite_hashtable(&(bottom_parasite_frame->prefix_table));
  parasite_hashtable_t* span_table = bottom_parasite_frame->prefix_table;

  fprintf(stderr, 
          "span_table->list_size = %d, span_table->table_size = %d, span_table->lg_capacity = %d\n",
      span_table->list_size, span_table->table_size, span_table->log_capacity);

  double work = bottom_function_frame->running_work + bottom_function_frame->local_work;
  flush_parasite_hashtable(&(main_stack->work_table));
  parasite_hashtable_t* work_table = main_stack->work_table;
  fprintf(stderr, 
          "work_table->list_size = %d, work_table->table_size = %d, work_table->lg_capacity = %d\n",
      work_table->list_size, work_table->table_size, work_table->log_capacity);

  // Open call site CSV
  sprintf(filename, "parasite_call_site_profile.csv");
  fout = fopen(filename, "w"); 

  // print the header for the csv file

  fprintf(fout, "file, line, call sites (call_site_ID), function type, ");
  fprintf(fout, "work on work, span on work, parallelism on work, count on work, ");
  fprintf(fout, "top work on work, top span on work, top parallelism on work, top count on work, ");
  fprintf(fout, "local work on work, local span on work, local parallelism on work, local count on work, ");
  fprintf(fout, "work on span, span on span, parallelism on span, count on span, ");
  fprintf(fout, "top work on span, top span on span, top parallelism on span, top count on span, ");
  fprintf(fout, "local work on span, local span on span, local parallelism on span, local count on span \n");

  // Parse tables
  int span_table_entries_read = 0;

  // TODO: replace i with and indexing system that works
  for (int i = 0; i < (1 << (work_table->log_capacity)); i++) {

    parasite_hashtable_entry_t *entry = &(work_table->entries[i]);

    if (hashtable_entry_is_empty(entry)) {
      i++;
      continue;
    }

    double work_work = entry->work;
    double span_work = entry->span;
    double par_work = (double)work_work/(double)span_work;
    double cnt_work = entry->count;

    double t_work_work = entry->top_work;
    double t_span_work = entry->top_span;
    double t_par_work = (double)t_work_work/(double)t_span_work;
    double t_cnt_work = entry->top_count;

    double l_work_work = entry->local_work;
    double l_span_work = entry->local_span;
    double l_par_work = (double)l_work_work/(double)l_span_work;
    double l_cnt_work = entry->local_call_site_invocation_count;

    double work_span = 0;
    double span_span = 0;
    double par_span = DBL_MAX;
    double cnt_span = 0;

    double t_work_span = 0;
    double t_span_span = 0;
    double t_par_span = DBL_MAX;
    double t_cnt_span = 0;

    double l_work_span = 0;
    double l_span_span = 0;
    double l_par_span = DBL_MAX;
    double l_cnt_span = 0;

    if (i < (1 << span_table->log_capacity)) {

      parasite_hashtable_entry_t *st_entry = &(span_table->entries[i]);

      if (!hashtable_entry_is_empty(st_entry)) {

        assert(st_entry->call_site_ID == entry->call_site_ID);

        work_span = st_entry->work;
        span_span = st_entry->span;
        par_span = (double)work_span / (double)span_span;
        cnt_span = st_entry->count;

        t_work_span = st_entry->top_work;
        t_span_span = st_entry->top_span;
        t_par_span = (double)t_work_span / (double)t_span_span;
        t_cnt_span = st_entry->top_count;

        l_work_span = st_entry->local_work;
        l_span_span = st_entry->local_span;
        l_par_span = (double)l_work_span / (double)l_span_span;
        l_cnt_span = st_entry->local_call_site_invocation_count;

        ++span_table_entries_read;
    }
  

    int line = i; 
    int call_site_ID = entry->call_site_ID;

    fprintf(fout, "\"%s\", %d, %d, ", filename, line, call_site_ID);

    int func_type = entry->func_type;

    if (main_stack->call_site_status_vector[i].flags & RECURSIVE) {  // recursive function
        fprintf(fout, "%s %s, ",
            FunctionType_str[func_type],
            FunctionType_str[IS_RECURSIVE]);
    } 

    else {
        fprintf(fout, "%s, ", FunctionType_str[func_type]);
    }
    
    fprintf(fout, "%f, %f, %g, %f, %f, %f, %g, %f, %f, %f, %g, %f, ", 
          work_work, span_work, par_work, cnt_work,
          t_work_work, t_span_work, t_par_work, t_cnt_work,
          l_work_work, l_span_work, l_par_work, l_cnt_work);

    fprintf(fout, "%f, %f, %g, %f, %f, %f, %g, %f, %f, %f, %g, %f\n", 
          work_span, span_span, par_span, cnt_span,
          t_work_span, t_span_span, t_par_span, t_cnt_span,
          l_work_span, l_span_span, l_par_span, l_cnt_span);
    }

    fclose(fout);

  }

    assert(span_table_entries_read == span_table->table_size);

    fprintf(stderr, "work = %fs, span = %fs, parallelism = %f\n",
    work / (1000000000.0),
    span / (1000000000.0),
    work / (float)span);
}

void return_of_called_operations(parasite_stack_t* main_stack, TIME return_time) {

    // Called G returns to F:
    // G.p += G.c
    // F.w += G.w
    // F.c += G.p
    // F.lock_span += G.lock_span

    const function_frame_t *bottom_function_frame= &(main_stack->function_stack[main_stack->function_stack_tail_index]);
  
    bool add_success;

    const function_frame_t *old_bottom_function_frame;
  
    // stop the timer and attribute the elapsed time to this returning
    // function

    // G.p += G.c
    TIME strand_len = return_time - last_strand_start;
    last_strand_start = return_time;
    main_stack->function_stack[stack->function_stack_tail_index].local_work += strand_len;
  
    assert(main_stack->function_stack_tail_index > main_stack->bottom_parasite_frame->head_function_index);
  
    // Pop the main_stack
    old_bottom_function_frame = parasite_function_pop(main_stack);
    double local_work = old_bottom_parasite_frame->local_work;
    double local_lock_span = old_bottom_parasite_frame->lock_span;


    double running_work = old_bottom_function_frame->running_work + local_work;
    double running_span = old_bottom_function_frame->running_span + local_work;
    double running_lock_span = old_bottom_function_frame->running_lock_span + local_lock_span;
  
    int32_t call_site_index = old_bottom_parasite_frame->call_site_index;
    int32_t cs_tail = main_stack->call_site_status_vector[call_site_index].call_site_tail_function_index;
    bool top_cs = (cs_tail == main_stack->function_stack_tail_index + 1);
  
    if (top_cs) {  // top call site instance
      main_stack->call_site_status_vector[call_site_index].call_site_tail_function_index = OFF_STACK;
      int32_t call_site_function_index = main_stack->call_site_status_vector[call_site_index].call_site_function_index;
      if (main_stack->function_status_vector[call_site_function_index] == main_stack->function_stack_tail_index + 1) {
        main_stack->function_status_vector[call_site_function_index] = OFF_STACK;
      }
    }
  
    function_frame_t *new_bottom_function_frame = &(main_stack->function_stack[main_stack->function_stack_tail_index]);

    // F.w += G.w
    // F.c += G.p
    // F.lock_span += G.lock_span
    new_bottom_function_frame->running_work += running_work;
    new_bottom_function_frame->running_span += running_span;
    new_bottom_function_frame->running_lock_span += running_lock_span;
  
    // Update work table
    if (top_cs) {
      uint32_t call_site_function_index = main_stack->call_site_status_vector[new_bottom_parasite_frame->call_site_index].call_site_function_index;
      /* fprintf(stderr, "adding to work table\n"); */
      add_success = add_to_parasite_hashtable(&(main_stack->work_table),
                                        main_stack->function_stack_tail_index == main_stack->function_status_vector[call_site_function_index],
                                        call_site_index,
                                        old_bottom_parasite_frame->call_site_index,
                                        running_work,
                                        running_span,
                                        local_work,
                                        local_work);
      assert(add_success);

      add_success = add_to_parasite_hashtable(&(main_stack->bottom_parasite_frame->continuation_table),
                                        main_stack->function_stack_tail_index == main_stack->function_status_vector[call_site_function_index],
                                        call_site_index,
                                        old_bottom_parasite_frame->call_site_index,
                                        running_work,
                                        running_span,
                                        local_work,
                                        local_work);
      assert(add_success);
    } else {

      // Only record the local work and local span
      /* fprintf(stderr, "adding to work table\n"); */
      add_success = add_local_to_parasite_hashtable(&(main_stack->work_table),
                                              call_site_index,
                                              old_bottom_parasite_frame->call_site_index,
                                              local_work,
                                              local_work);
      assert(add_success);
      /* fprintf(stderr, "adding to contin table\n"); */
      add_success = add_local_to_parasite_hashtable(&(main_stack->bottom_parasite_frame->continuation_table),
                                              call_site_index,
                                              old_bottom_parasite_frame->call_site_index,
                                              local_work,
                                              local_work);
      assert(add_success);
    }
}


void thread_end_operations(parasite_stack_t* main_stack, TIME thread_end_time) {

  // Created G returns to F
  // G.p += G.c
  // F.w += G.w
  // F.lock_span += G.lock_span
  // if F.c + G.p > F.l
  //    F.l = G.p
  //    F.longest_child_lock_span = G.lock_span 
  //    F.p += G.c
  //    F.c = 0

  parasite_stack_frame_t *old_bottom_parasite_frame;
  bool add_success;

  // G.p += G.c
  TIME strand_len = thread_end_time - last_strand_start;
  last_strand_start = thread_end_time;
  main_stack->function_stack[stack->function_stack_tail_index].local_work += strand_len;

  // F.w += G.w
  main_stack->bottom_parasite_frame->local_continuation += strand_len;

  // We are leaving this function, so it must have sync-ed, meaning
  // that, lchild should be 0 / empty.  prefix could contain value,
  // however, if this function is a Cilk function that spawned before.
  assert(main_stack->bottom_parasite_frame->longest_child_span == 0.0);
  assert(parasite_hashtable_is_empty(main_stack->bottom_parasite_frame->longest_child_table));

  assert(main_stack->bottom_parasite_frame->head_function_index == main_stack->function_stack_tail_index);

  function_frame_t *old_bottom_function_frame= &(main_stack->function_stack[main_stack->function_stack_tail_index]);

  main_stack->bottom_parasite_frame->prefix_span += old_bottom_function_frame->running_span;
  main_stack->bottom_parasite_frame->local_span += main_stack->bottom_parasite_frame->local_continuation + BURDENING;
  old_bottom_function_frame->running_work += old_bottom_function_frame->local_work;
  main_stack->bottom_parasite_frame->prefix_span += main_stack->bottom_parasite_frame->local_span;

  add_parasite_hashtables(&(main_stack->bottom_parasite_frame->prefix_table), &(main_stack->bottom_parasite_frame->continuation_table));

  // Pop the main_stack
  old_bottom_parasite_frame = parasite_stack_pop(main_stack);

  function_frame_t *bottom_function_frame= &(main_stack->function_stack[main_stack->function_stack_tail_index]);

  int32_t call_site_index = old_bottom_function_frame->call_site_index;
  int32_t cs_tail = main_stack->call_site_status_vector[call_site_index].call_site_tail_function_index;
  bool top_cs = (cs_tail == main_stack->function_stack_tail_index + 1);

  if (top_cs) {  // top CS instance
    main_stack->call_site_status_vector[call_site_index].call_site_tail_function_index = OFF_STACK;
    int32_t call_site_function_index = main_stack->call_site_status_vector[call_site_index].call_site_function_index;
    if (main_stack->function_status_vector[call_site_function_index] == main_stack->function_stack_tail_index + 1) {
      main_stack->function_status_vector[call_site_function_index] = OFF_STACK;
    }
  }

  bottom_function_frame->running_work += old_bottom_function_frame->running_work;

  // Update work table
  if (top_cs) {

    int32_t call_site_function_index = main_stack->call_site_status_vector[bottom_function_frame->call_site_index].call_site_function_index;
    add_success = add_to_parasite_hashtable(&(main_stack->work_table),
                                      main_stack->function_stack_tail_index == main_stack->function_status_vector[call_site_function_index],
                                      call_site_index,
                                      old_bottom_function_frame->call_site_index,
                                      old_bottom_function_frame->running_work,
                                      old_bottom_parasite_frame->prefix_span,
                                      old_bottom_function_frame->local_work,
                                      old_bottom_parasite_frame->local_span);
    assert(add_success);


    add_success = add_to_parasite_hashtable(&(old_bottom_parasite_frame->prefix_table),
                                      main_stack->function_stack_tail_index == main_stack->function_status_vector[call_site_function_index],
                                      call_site_index,
                                      old_bottom_function_frame->call_site_index,
                                      old_bottom_function_frame->running_work,
                                      old_bottom_parasite_frame->prefix_span,
                                      old_bottom_function_frame->local_work,
                                      old_bottom_parasite_frame->local_span);
    assert(add_success);
  } 

  else {

    // Only record the local work and local span
    add_success = add_local_to_parasite_hashtable(&(main_stack->work_table),
                                            call_site_index,
                                            old_bottom_function_frame->call_site_index,
                                            old_bottom_function_frame->local_work,
                                            old_bottom_parasite_frame->local_span);
    assert(add_success);

    add_success = add_local_to_parasite_hashtable(&(old_bottom_parasite_frame->prefix_table),
                                            call_site_index,
                                            old_bottom_function_frame->call_site_index,
                                            old_bottom_function_frame->local_work,
                                            old_bottom_parasite_frame->local_span);
    assert(add_success);
  }


  // This is the case we are returning to a spawn, since a HELPER 
  // is always invoked due to a spawn statement.

    assert(HELPER != main_stack->bottom_parasite_frame->func_type);

    // if F.c + G.p > F.l
    if (bottom_function_frame->running_span + old_bottom_parasite_frame->prefix_span > main_stack->bottom_parasite_frame->longest_child_span) {

      // F.p += G.c
      main_stack->bottom_parasite_frame->prefix_span += bottom_function_frame->running_span;
      main_stack->bottom_parasite_frame->local_span += main_stack->bottom_parasite_frame->local_continuation;

      // This needs a better data structure to be implemented more
      // efficiently.
      add_parasite_hashtables(&(main_stack->bottom_parasite_frame->prefix_table), &(main_stack->bottom_parasite_frame->continuation_table));

      // Save old_bottom_parasite_frame tables in new bottom_parasite_frame's l_child variable.

       // F.l = G.p
      main_stack->bottom_parasite_frame->longest_child_span = old_bottom_parasite_frame->prefix_span;
      // F.longest_child_lock_span = G.lock_span 
      main_stack->bottom_parasite_frame->longest_child_lock_span = old_bottom_parasite_frame->lock_span; 

      clear_parasite_hashtable(main_stack->bottom_parasite_frame->longest_child_table);

      parasite_hashtable_t* tmp_hashtable = main_stack->bottom_parasite_frame->longest_child_table;

      main_stack->bottom_parasite_frame->longest_child_table = old_bottom_parasite_frame->prefix_table;
      old_bottom_parasite_frame->prefix_table = tmp_hashtable;

      // Free old_bottom_parasite_frame tables that are no longer in use
      clear_parasite_hashtable(old_bottom_parasite_frame->longest_child_table);
      clear_parasite_hashtable(old_bottom_parasite_frame->continuation_table);

      // Empy new bottom_parasite_frame's continuation
      bottom_function_frame->running_span = 0;
      main_stack->bottom_parasite_frame->local_continuation = 0;
      clear_parasite_hashtable(main_stack->bottom_parasite_frame->continuation_table);

    } else {
      // Discared all tables from old_bottom_parasite_frame
      clear_parasite_hashtable(old_bottom_parasite_frame->prefix_table);
      clear_parasite_hashtable(old_bottom_parasite_frame->longest_child_table);
      clear_parasite_hashtable(old_bottom_parasite_frame->continuation_table);

    }

    old_bottom_parasite_frame->parent = main_stack->stack_frame_free_list;
    main_stack->stack_frame_free_list = old_bottom_parasite_frame;
}




