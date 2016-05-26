// Auxiliary functions for Parasite Tool

#include "ParasiteStacks.h"

__attribute__((always_inline))
void begin_strand(parasite_stack_t *stack) {
  start_strand(&(stack->strand_ruler));
}

__attribute__((always_inline))
uint64_t measure_and_add_strand_length(parasite_stack_t *stack) {
  // Measure strand length

  int strand_len = measure_strand_length(&(stack->strand_ruler));
  assert(NULL != stack->bot);

  // Accumulate strand length
  stack->function_stack[stack->function_stack_tail].local_work += strand_len;
  return strand_len;
}

void parasite_tool_print(parasite_stack_t *stack)
{
  FILE *fout;
  char filename[64];

  assert(NULL != stack->bot);
  assert(MAIN == stack->bot->func_type);
  assert(stack->bot->head_function_signature == stack->function_stack_tail);

  cilkprof_stack_frame_t *bottom = stack->bot;
  c_fn_frame_t *c_bottom = &(stack->c_stack[stack->c_tail]);

  uint64_t span = bottom->prefix_span + c_bottom->running_span
      + bottom->local_span + bottom->local_continuation;

  add_cc_hashtables(&(bottom->prefix_table), &(bottom->continuation_table));
  clear_cc_hashtable(bottom->continuation_table);

  flush_cc_hashtable(&(bottom->prefix_table));

  cc_hashtable_t* span_table = bottom->prefix_table;
  fprintf(stderr, 
          "span_table->list_size = %d, span_table->table_size = %d, span_table->lg_capacity = %d\n",
      span_table->list_size, span_table->table_size, span_table->lg_capacity);

  uint64_t work = c_bottom->running_work + c_bottom->local_work;
  flush_cc_hashtable(&(stack->work_table));
  cc_hashtable_t* work_table = stack->work_table;
  fprintf(stderr, 
          "work_table->list_size = %d, work_table->table_size = %d, work_table->lg_capacity = %d\n",
      work_table->list_size, work_table->table_size, work_table->lg_capacity);

  // Read the proc maps list
  read_proc_maps();

  // Open call site CSV
  sprintf(filename, "cilkprof_cs_%d.csv", TOOL_PRINT_NUM);
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
    while (NULL != record /* && (uintptr_t)NULL != record->iaddr */) {

      assert(0 != record->iaddr);
      assert(0 <= record->index && record->index < (1 << work_table->lg_capacity));

      cc_hashtable_entry_t *entry = &(work_table->entries[ record->index ]);


      if (empty_cc_entry_p(entry)) {
        record = record->next;
        continuationue;
      }

      assert(entry->rip == record->iaddr);

      uint64_t work_work = entry->work;
      uint64_t span_work = entry->span;
      double par_work = (double)work_work/(double)span_work;
      uint64_t cnt_work = entry->count;

      uint64_t t_work_work = entry->top_work;
      uint64_t t_span_work = entry->top_span;
      double t_par_work = (double)t_work_work/(double)t_span_work;
      uint64_t t_cnt_work = entry->top_count;

      uint64_t l_work_work = entry->local_work;
      uint64_t l_span_work = entry->local_span;
      double l_par_work = (double)l_work_work/(double)l_span_work;
      uint64_t l_cnt_work = entry->local_count;

      uint64_t work_span = 0;
      uint64_t span_span = 0;
      double par_span = DBL_MAX;
      uint64_t cnt_span = 0;

      uint64_t t_work_span = 0;
      uint64_t t_span_span = 0;
      double t_par_span = DBL_MAX;
      uint64_t t_cnt_span = 0;

      uint64_t l_work_span = 0;
      uint64_t l_span_span = 0;
      double l_par_span = DBL_MAX;
      uint64_t l_cnt_span = 0;

      if (record->index < (1 << span_table->lg_capacity)) {
        cc_hashtable_entry_t *st_entry = &(span_table->entries[ record->index ]);

        if (!empty_cc_entry_p(st_entry)) {
          assert(st_entry->rip == entry->rip);

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
          l_cnt_span = st_entry->local_count;

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
              work_work, span_work, par_work, cnt_work,
              t_work_work, t_span_work, t_par_work, t_cnt_work,
              l_work_work, l_span_work, l_par_work, l_cnt_work);
      fprintf(fout, "%lu, %lu, %g, %lu, %lu, %lu, %g, %lu, %lu, %lu, %g, %lu\n", 
              work_span, span_span, par_span, cnt_span,
              t_work_span, t_span_span, t_par_span, t_cnt_span,
              l_work_span, l_span_span, l_par_span, l_cnt_span);
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

}


