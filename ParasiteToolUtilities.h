static inline void initialize_tool(cilkprof_stack_t *stack) {

  cilkprof_stack_init(stack, MAIN);
  call_site_table = iaddr_table_create();
  function_table = iaddr_table_create();
  TOOL_INITIALIZED = true;
  TOOL_PRINTED = false;
}

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
  stack->c_stack[stack->c_tail].local_wrk += strand_len;
  return strand_len;
}
