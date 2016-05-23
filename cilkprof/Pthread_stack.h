#ifndef INCLUDED_CILKPROF_STACK_H
#define INCLUDED_CILKPROF_STACK_H

#include <stdbool.h>
#include <time.h>
#include <limits.h>

#include "cc_hashtable.h"

// Used to size call site and function status vectors
const int START_STATUS_VECTOR_SIZE = 4;
const int START_C_STACK_SIZE = 8;

typedef struct c_fn_frame_t {

  // Index for this call site
  int32_t cs_index;

  // Return address of this function
  uintptr_t rip;
  // Address of this function
  uintptr_t function;

  // Work and span values to store for every function
  uint64_t local_wrk;
  /* uint64_t local_contin; */

  uint64_t running_wrk;
  uint64_t running_spn;

  /* // Parent of this C function on the same stack */
  /* struct c_fn_frame_t *parent; */
} c_fn_frame_t __attribute__((aligned(16)));

// Type for pthreadprof stack frame
typedef struct pthreadprof_stack_frame_t {

  // Function type
  FunctionType_t func_type;

  // Index of head C function in stack
  int32_t c_head;

  // Local continuation span of this function
  uint64_t local_contin;
  // Local span of this function
  uint64_t local_spn;
  // Local lock span of this function
  uint64_t local_lock_spn;

  // Span of the prefix of this function and its child C functions
  uint64_t prefix_spn;

  // Span of the longest spawned child of this function observed so
  // far
  uint64_t lchild_spn;

  // runtime at beginning of most recent lock encountered by function
  double last_lock_start;

  // The span of the continuation is stored in the running_spn
  // variable in the topmost c_fn_frame

  // Data associated with the function's prefix
  cc_hashtable_t* prefix_table;

  // Data associated with the function's longest child
  cc_hashtable_t* lchild_table;

  // Data associated with the function's continuation
  cc_hashtable_t* contin_table;

  // Data associated with the function's lock span
  cc_hashtable_t* lock_spn_table;

  // Data associated with the function's longest child lock span
  cc_hashtable_t* lchild_lock_spn_table;
 
  // Data associated with the function's last lock start
  cc_hashtable_t* last_lock_start_table;

  // Pointer to the frame's parent
  struct pthreadprof_stack_frame_t *parent;
} pthreadprof_stack_frame_t;


// Metadata for a call site
typedef struct {
  /* uint32_t count_on_stack; */
  /* FunctionType_t func_type; */
  int32_t c_tail;
  int32_t fn_index;
  uint32_t flags;
} cs_status_t;

// Metadata for a function
typedef int32_t fn_status_t;

const uint32_t RECURSIVE = 1;
const int32_t OFF_STACK = INT32_MIN;
const int32_t UNINITIALIZED = INT32_MIN;

// Type for a pthreadprof stack
typedef struct {

  // Capacity of call-site status vector
  int cs_status_capacity;

  // Capacity of function status vector
  int fn_status_capacity;

  // Capacity of C stack
  int c_stack_capacity;

  // Current bottom of C stack
  int32_t c_tail;

  // Stack of C function frames
  c_fn_frame_t *c_stack;

  // Vector of status flags for different call sites
  cs_status_t *cs_status;

  // Vector of status flags for different functions
  fn_status_t *fn_status;

  // Pointer to bottom of the stack, onto which frames are pushed.
  pthreadprof_stack_frame_t *bot;

  // Call-site data associated with the running work
  cc_hashtable_t* wrk_table;

  // Free list of pthreadprof stack frames
  pthreadprof_stack_frame_t *sf_free_list;
} pthreadprof_stack_t;


/*----------------------------------------------------------------------*/

// Resizes the C stack
__attribute__((always_inline))
void resize_c_stack(c_fn_frame_t **c_stack, int *c_stack_capacity) {
  int new_c_stack_capacity = 2 * (*c_stack_capacity);
  c_fn_frame_t *new_c_stack = (c_fn_frame_t*)malloc(sizeof(c_fn_frame_t)
                                                    * new_c_stack_capacity);
  for (int i = 0; i < *c_stack_capacity; ++i) {
    new_c_stack[i] = (*c_stack)[i];
  }

  free(*c_stack);
  *c_stack = new_c_stack;
  *c_stack_capacity = new_c_stack_capacity;
}

// Initializes C function frame *c_fn_frame
static inline
void pthreadprof_c_fn_frame_init(c_fn_frame_t *c_fn_frame) {
  /* c_fn_frame->top_cs = false; */
  /* c_fn_frame->top_fn = false; */

  c_fn_frame->cs_index = 0;
  /* c_fn_frame->fn_index = 0; */

#ifndef NDEBUG
  c_fn_frame->rip = (uintptr_t)NULL;
  c_fn_frame->function = (uintptr_t)NULL;
#endif

  c_fn_frame->local_wrk = 0;
  /* c_fn_frame->local_contin = 0; */
  c_fn_frame->running_wrk = 0;
  c_fn_frame->running_spn = 0;

  /* c_fn_frame->parent = NULL; */
}

// Initializes the pthreadprof stack frame *frame
static inline
void pthreadprof_stack_frame_init(pthreadprof_stack_frame_t *frame,
                               FunctionType_t func_type,
                               int c_head)
{
  frame->parent = NULL;
  frame->func_type = func_type;
  frame->c_head = c_head;
  frame->local_spn = 0;
  frame->local_contin = 0;
  frame->prefix_spn = 0; 
  frame->lchild_spn = 0;

  assert(cc_hashtable_is_empty(frame->prefix_table));

  assert(cc_hashtable_is_empty(frame->lchild_table));

  assert(cc_hashtable_is_empty(frame->contin_table));



// Push new frame of C function onto the C function stack starting at
// stack->bot->c_fn_frame.
c_fn_frame_t* pthreadprof_c_fn_push(pthreadprof_stack_t *stack)
{
  assert(NULL != stack->bot);

  ++stack->c_tail;

  if (stack->c_tail >= stack->c_stack_capacity) {
    resize_c_stack(&(stack->c_stack), &(stack->c_stack_capacity));
  }

  pthreadprof_c_fn_frame_init(&(stack->c_stack[stack->c_tail]));

  return &(stack->c_stack[stack->c_tail]);
}


// Push new frame of function type func_type onto the stack *stack
__attribute__((always_inline))
pthreadprof_stack_frame_t*
pthreadprof_stack_push(pthreadprof_stack_t *stack, FunctionType_t func_type)
{
  pthreadprof_stack_frame_t *new_frame;
  if (NULL != stack->sf_free_list) {
    new_frame = stack->sf_free_list;
    stack->sf_free_list = stack->sf_free_list->parent;
  } else {
    new_frame = (pthreadprof_stack_frame_t *)malloc(sizeof(pthreadprof_stack_frame_t));

    new_frame->prefix_table = cc_hashtable_create();
    new_frame->lchild_table = cc_hashtable_create();
    new_frame->contin_table = cc_hashtable_create();

  }

  pthreadprof_c_fn_push(stack);
  pthreadprof_stack_frame_init(new_frame, func_type, stack->c_tail);
  new_frame->parent = stack->bot;
  stack->bot = new_frame;

  return new_frame;
}


// Initializes the pthreadprof stack
void pthreadprof_stack_init(pthreadprof_stack_t *stack, FunctionType_t func_type)
{
  stack->in_user_code = false;
  stack->bot = NULL;
  stack->sf_free_list = NULL;

  stack->c_stack = (c_fn_frame_t*)malloc(sizeof(c_fn_frame_t) * START_C_STACK_SIZE);
  stack->c_stack_capacity = START_C_STACK_SIZE;
  stack->c_tail = 0;

  pthreadprof_stack_frame_t *new_frame = (pthreadprof_stack_frame_t *)malloc(sizeof(pthreadprof_stack_frame_t));
  new_frame->prefix_table = cc_hashtable_create();
  new_frame->lchild_table = cc_hashtable_create();
  new_frame->contin_table = cc_hashtable_create();


  pthreadprof_stack_frame_init(new_frame, func_type, 0);
  pthreadprof_c_fn_frame_init(&(stack->c_stack[0]));

  stack->bot = new_frame;
  stack->wrk_table = cc_hashtable_create();
  stack->cs_status_capacity = START_STATUS_VECTOR_SIZE;
  stack->fn_status_capacity = START_STATUS_VECTOR_SIZE;

  stack->cs_status = (cs_status_t*)malloc(sizeof(cs_status_t)
                                          * START_STATUS_VECTOR_SIZE);

  stack->fn_status = (fn_status_t*)malloc(sizeof(fn_status_t)
                                          * START_STATUS_VECTOR_SIZE);

  for (int i = 0; i < START_STATUS_VECTOR_SIZE; ++i) {

    stack->cs_status[i].c_tail = OFF_STACK;
    stack->cs_status[i].fn_index = UNINITIALIZED;
    stack->cs_status[i].flags = 0;
    stack->fn_status[i] = OFF_STACK;
  }

  init_strand_ruler(&(stack->strand_ruler));
}

// Doubles the capacity of a cs status vector
void resize_cs_status_vector(cs_status_t **old_status_vec,
                             int *old_vec_capacity) {
  int new_vec_capacity = *old_vec_capacity * 2;
  cs_status_t *new_status_vec = (cs_status_t*)malloc(sizeof(cs_status_t)
                                                     * new_vec_capacity);
  int i;
  for (i = 0; i < *old_vec_capacity; ++i) {
    new_status_vec[i] = (*old_status_vec)[i];
  }
  for ( ; i < new_vec_capacity; ++i) {
    /* new_status_vec[i].count_on_stack = 0; */
    new_status_vec[i].c_tail = OFF_STACK;
    new_status_vec[i].fn_index = UNINITIALIZED;
    new_status_vec[i].flags = 0;
  }

  free(*old_status_vec);
  *old_status_vec = new_status_vec;
  *old_vec_capacity = new_vec_capacity;
}

// Doubles the capacity of a fn status vector
void resize_fn_status_vector(fn_status_t **old_status_vec,
                             int *old_vec_capacity) {
  int new_vec_capacity = *old_vec_capacity * 2;
  fn_status_t *new_status_vec = (fn_status_t*)malloc(sizeof(fn_status_t)
                                                     * new_vec_capacity);
  int i;
  for (i = 0; i < *old_vec_capacity; ++i) {
    new_status_vec[i] = (*old_status_vec)[i];
  }
  for ( ; i < new_vec_capacity; ++i) {
    new_status_vec[i] = OFF_STACK;
  }

  free(*old_status_vec);
  *old_status_vec = new_status_vec;
  *old_vec_capacity = new_vec_capacity;
}

// Pops the bottommost C frame off of the stack
// stack->bot->c_fn_frame, and returns a pointer to it.
c_fn_frame_t* pthreadprof_c_fn_pop(pthreadprof_stack_t *stack)
{
  c_fn_frame_t *old_c_bot = &(stack->c_stack[stack->c_tail]);
  --stack->c_tail;
  assert(stack->c_tail >= stack->bot->c_head);
  return old_c_bot;
}


// Pops the bottommost frame off of the stack *stack, and returns a
// pointer to it.
pthreadprof_stack_frame_t* pthreadprof_stack_pop(pthreadprof_stack_t *stack)
{
  pthreadprof_stack_frame_t *old_bottom = stack->bot;
  stack->bot = stack->bot->parent;
  pthreadprof_c_fn_pop(stack);
  return old_bottom;
}

#endif
