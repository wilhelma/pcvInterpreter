#ifndef _PARASITE_STACKS_H_
#define _PARASITE_STACKS_H_

#include "ParasiteHashtable.h"
#include "assert.h"

// Used to size call site status vector 
const int START_FUNCTION_STATUS_VECTOR_SIZE = 4;

// Used to size function call site stack
const int START_CALL_SITE_STACK_SIZE = 8;

// frame for each function
typedef struct function_frame_t {

  // ID for the function's call site
  CALLSITE call_site_ID;

  // index for the function's call site
  int call_site_index;

  // Signature for the function
  FUN_SG functionSignature;

  // Work and span values to store for every function
  TIME local_work;
  TIME local_span;
  TIME local_lock_span;
  TIME last_lock_start;

  // Running total of work and span for this call site
  TIME running_work;
  TIME running_span;
  TIME running_lock_span;

  // Parent of this function on the same stack 
  struct function_frame_t *parent;

} function_frame_t;

// Type for thread stack frame
typedef struct thread_stack_frame_t {

  // Thread ID 
  TRD_ID threadId;

  // Pointer to the frame's parent
  struct thread_stack_frame_t *parent;

} thread_frame_t;

// Type for parasite stack frame
typedef struct parasite_stack_frame_t {

  // Function type
  int func_type;

  // Index of head C function in stack
  int head_function_index;

  // Index of head thread in stack
  int head_thread_index;

  // Signature of head function in call stack
  FUN_SG headFunctionSignature;

  // ID of head thread in thread stack
  TRD_ID headThreadID;

  // Local continuation span of this function
  TIME local_continuation;

  // Local span of this function
  TIME local_span;

  // Span of the prefix of this function and its child C functions
  TIME prefix_span;

  // lock span of this function
  TIME lock_span;

  // Span of the longest spawned child of this function observed so
  // far
  TIME longest_child_span;

  // Span of the longest spawned child of this function observed so
  // far
  TIME longest_child_lock_span;

  // The span of the continuation is stored in the running_span
  // variable in the topmost function_frame

  // Data associated with the function's prefix
  parasite_hashtable_t* prefix_table;

  // Data associated with the function's longest child
  parasite_hashtable_t* longest_child_table;

  // Data associated with the function's continuation
  parasite_hashtable_t* continuation_table;

  // Pointer to the frame's parent
  struct parasite_stack_frame_t *parent;

} parasite_stack_frame_t;

// Metadata for a call site
typedef struct {

  FUN_SG tailFunctionSignature;
  int call_site_tail_function_index;

  FUN_SG fnSignature;
  int call_site_function_index;

  int last_index_used;

  uint32_t flags;
} call_site_status_t;

// Metadata for a function
typedef int32_t function_status_t;

// Metadata for a thread
typedef int32_t thread_status_t;

const uint32_t RECURSIVE = 1;
const int32_t OFF_STACK = INT32_MIN;
const int32_t UNINITIALIZED = INT32_MIN;

// Type for a parasite stack
typedef struct {

  // Capacity of call-site status vector
  int call_site_status_vector_capacity;

  // Capacity of function status vector
  int function_status_vector_capacity;

  // Capacity of thread status vector
  int thread_status_vector_capacity;

  // Capacity of C stack
  int function_stack_capacity;

  // Current bottom of C stack
  int function_stack_tail_index;

  // Capacity of thread stack
  int thread_stack_capacity;

  // Current bottom of thread stack
  int thread_stack_tail_index;

  // Stack of C function frames
  function_frame_t *function_stack;

  // Stack of thread frames
  thread_frame_t *thread_stack;

  // Vector of status flags for different call sites
  call_site_status_t *call_site_status_vector;

  // Vector of status flags for different functions
  function_status_t *function_status_vector;

  // Vector of status flags for different threads
  thread_status_t *thread_status_vector;

  // Pointer to bottom of the stack, onto which frames are pushed.
  parasite_stack_frame_t *bottom_parasite_frame;

  // Call-site data associated with the running work
  parasite_hashtable_t* work_table;

  // Free list of parasite stack frames
  parasite_stack_frame_t *stack_frame_free_list;

} parasite_stack_t;


/*----------------------------------------------------------------------*/

// Resizes the C stack
__attribute__((always_inline))
void resize_function_stack(function_frame_t **function_stack, int *function_stack_capacity) {
  int new_function_stack_capacity = 2 * (*function_stack_capacity);
  function_frame_t *new_function_stack = (function_frame_t*)malloc(sizeof(function_frame_t)
                                                    * new_function_stack_capacity);
  for (int i = 0; i < *function_stack_capacity; ++i) {
    new_function_stack[i] = (*function_stack)[i];
  }

  free(*function_stack);
  *function_stack = new_function_stack;
  *function_stack_capacity = new_function_stack_capacity;

}


__attribute__((always_inline))
void resize_thread_stack(thread_frame_t **thread_stack, int *thread_stack_capacity) {
  int new_thread_stack_capacity = 2 * (*thread_stack_capacity);
  thread_frame_t *new_thread_stack = (thread_frame_t*)malloc(sizeof(thread_frame_t)
                                                    * new_thread_stack_capacity);
  for (int i = 0; i < *thread_stack_capacity; ++i) {
    new_thread_stack[i] = (*thread_stack)[i];
  }

  free(*thread_stack);
  *thread_stack = new_thread_stack;
  *thread_stack_capacity = new_thread_stack_capacity;

}


// Initializes C function frame *function_frame
static inline
void function_frame_init(function_frame_t *function_frame) {//, CALLSITE csID, int call_site_index, FUN_SG funSg)

  // Signature for the function
  FUN_SG functionSignature;

  // function_frame->call_site_ID = csID;
  // function_frame->call_site_index = call_site_index;
  // function_frame->functionSignature = funSg;

  function_frame->local_work = 0;
  function_frame->running_work = 0;
  function_frame->running_span = 0;
}

// Initializes the parasite stack frame *frame
static inline
void parasite_stack_frame_init(parasite_stack_frame_t *frame,
                               int func_type,
                               int head_function_index,
                               int head_thread_index) // 
                               //FUN_SG functionSignature,
                               // TRD_ID threadID)
{
  frame->parent = NULL;
  frame->func_type = func_type;
  frame->headFunctionSignature = (FUN_SG) 0;
  frame->headThreadID = (TRD_ID) 0;

  frame->head_function_index = head_function_index;
  frame->head_thread_index = head_thread_index;

  frame->local_span = 0;
  frame->local_continuation = 0;
  frame->prefix_span = 0; 
  frame->longest_child_span = 0;


  assert(parasite_hashtable_is_empty(frame->prefix_table));
  assert(parasite_hashtable_is_empty(frame->longest_child_table));
  assert(parasite_hashtable_is_empty(frame->continuation_table));

}


// Push new frame of C function onto the C function stack starting at
// stack->bottom->function_frame.
function_frame_t* function_push(parasite_stack_t *stack)
{
  /* fprintf(stderr, "pushing C stack\n"); */
  assert(NULL != stack->bottom_parasite_frame);

  ++stack->function_stack_tail_index;

  if (stack->function_stack_tail_index >= stack->function_stack_capacity) {
    resize_function_stack(&(stack->function_stack), &(stack->function_stack_capacity));
  }

  function_frame_init(&(stack->function_stack[stack->function_stack_tail_index]));
 
  return &(stack->function_stack[stack->function_stack_tail_index]);
}

// Pops the bottommost thread frame off of the thread stack, and returns a pointer to it.
thread_frame_t* thread_push(parasite_stack_t *stack)
{
  assert(NULL != stack->bottom_parasite_frame);

  ++stack->thread_stack_tail_index;

  if (stack->thread_stack_tail_index >= stack->thread_stack_capacity) {
    resize_thread_stack(&(stack->thread_stack), &(stack->thread_stack_capacity));
  } 

  return &(stack->thread_stack[stack->thread_stack_tail_index]);
}

// Push new frame of function type func_type onto the stack *stack
__attribute__((always_inline))
parasite_stack_frame_t*
parasite_stack_push(parasite_stack_t *stack, int func_type)
{
  parasite_stack_frame_t *new_frame;
  if (NULL != stack->stack_frame_free_list) {
    new_frame = stack->stack_frame_free_list;
    stack->stack_frame_free_list = stack->stack_frame_free_list->parent;

  } else {

    new_frame = (parasite_stack_frame_t *)malloc(sizeof(parasite_stack_frame_t));
    new_frame->prefix_table = parasite_hashtable_create();
    new_frame->longest_child_table = parasite_hashtable_create();
    new_frame->continuation_table = parasite_hashtable_create();

  }

  function_push(stack);
  parasite_stack_frame_init(new_frame, func_type, stack->function_stack_tail_index, stack->thread_stack_tail_index);
  new_frame->parent = stack->bottom_parasite_frame;
  stack->bottom_parasite_frame = new_frame;
  return new_frame;
}


// Initializes the parasite stack
void parasite_stack_init(parasite_stack_t *stack, int func_type)
{
  stack->bottom_parasite_frame = NULL;
  stack->stack_frame_free_list = NULL;

  stack->function_stack = (function_frame_t*)malloc(sizeof(function_frame_t) * START_FUNCTION_STATUS_VECTOR_SIZE);
  stack->function_stack_capacity = START_FUNCTION_STATUS_VECTOR_SIZE;
  stack->function_stack_tail_index = 0;

  stack->thread_stack = (thread_frame_t*)malloc(sizeof(thread_frame_t) * START_FUNCTION_STATUS_VECTOR_SIZE);
  stack->thread_stack_capacity = START_FUNCTION_STATUS_VECTOR_SIZE;
  stack->thread_stack_tail_index = 0;

  parasite_stack_frame_t *new_frame = (parasite_stack_frame_t *)malloc(sizeof(parasite_stack_frame_t));

  new_frame->prefix_table = parasite_hashtable_create();
  new_frame->longest_child_table = parasite_hashtable_create();
  new_frame->continuation_table = parasite_hashtable_create();

  parasite_stack_frame_init(new_frame, func_type, 0, 0);
  function_frame_init(&(stack->function_stack[0]));

  stack->bottom_parasite_frame = new_frame;

  stack->work_table = parasite_hashtable_create();
  stack->call_site_status_vector_capacity = START_FUNCTION_STATUS_VECTOR_SIZE;
  stack->function_stack_capacity = START_FUNCTION_STATUS_VECTOR_SIZE;
  stack->thread_stack_capacity = START_FUNCTION_STATUS_VECTOR_SIZE;

  stack->call_site_status_vector = (call_site_status_t*)malloc(sizeof(call_site_status_t)
                                          * START_FUNCTION_STATUS_VECTOR_SIZE);

  stack->function_status_vector = (function_status_t*)malloc(sizeof(function_status_t)
                                          * START_FUNCTION_STATUS_VECTOR_SIZE);

  stack->thread_status_vector = (thread_status_t*)malloc(sizeof(thread_status_t)
                                          * START_FUNCTION_STATUS_VECTOR_SIZE);


  for (int i = 0; i < START_FUNCTION_STATUS_VECTOR_SIZE; ++i) {

    stack->call_site_status_vector[i].call_site_tail_function_index = OFF_STACK;
    stack->call_site_status_vector[i].call_site_function_index = UNINITIALIZED;
    stack->call_site_status_vector[i].flags = 0;
    stack->function_status_vector[i] = OFF_STACK;
    stack->thread_status_vector[i] = OFF_STACK;
  }
}

// Doubles the capacity of a call_site status vector
void resize_call_site_status_vector(call_site_status_t **old_status_vec,
                             int *old_vec_capacity) {
  int new_vec_capacity = *old_vec_capacity * 2;
  call_site_status_t *new_status_vec = (call_site_status_t*)malloc(sizeof(call_site_status_t)
                                                     * new_vec_capacity);
  int i;
  for (i = 0; i < *old_vec_capacity; ++i) {
    new_status_vec[i] = (*old_status_vec)[i];
  }
  for ( ; i < new_vec_capacity; ++i) {
    /* new_status_vec[i].count_on_stack = 0; */
    new_status_vec[i].call_site_tail_function_index = OFF_STACK;
    new_status_vec[i].call_site_function_index = UNINITIALIZED;
    new_status_vec[i].flags = 0;
  }

  free(*old_status_vec);
  *old_status_vec = new_status_vec;
  *old_vec_capacity = new_vec_capacity;
}

// Doubles the capacity of a function status vector
void resize_function_status_vector(function_status_t **old_status_vec,
                             int *old_vec_capacity) {
  int new_vec_capacity = *old_vec_capacity * 2;
  function_status_t *new_status_vec = (function_status_t*)malloc(sizeof(function_status_t)
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

void resize_thread_status_vector(thread_status_t **old_status_vector, 
                                 int *old_vector_capacity) {

  int new_vector_capacity = *old_vector_capacity * 2;
  thread_status_t *new_status_vector = (thread_status_t*)malloc(sizeof(thread_status_t)
                                                     * new_vector_capacity);
  int i;
  for (i = 0; i < *old_vector_capacity; ++i) {
    new_status_vector[i] = (*old_status_vector)[i];
  }
  for ( ; i < new_vector_capacity; ++i) {
    new_status_vector[i] = OFF_STACK;
  }

  free(*old_status_vector);
  *old_status_vector = new_status_vector;
  *old_vector_capacity = new_vector_capacity;
}

// Pops the bottommost thread frame off of the thread stack, and returns a pointer to it.
thread_stack_frame_t* thread_pop(parasite_stack_t *main_stack)
{
  thread_frame_t *old_thread_stack_bottom = &(main_stack->thread_stack[main_stack->thread_stack_tail_index]);
  --main_stack->thread_stack_tail_index;
  assert(main_stack->thread_stack_tail_index >= main_stack->bottom_parasite_frame->head_thread_index);
  return old_thread_stack_bottom;
}


// Pops the bottommost C frame off of the stack
// stack->bottom->function_frame, and returns a pointer to it.
function_frame_t* function_pop(parasite_stack_t *main_stack)
{
  function_frame_t *old_function_stack_bottom = &(main_stack->function_stack[main_stack->function_stack_tail_index]);
  --main_stack->function_stack_tail_index;
  assert(main_stack->function_stack_tail_index >= main_stack->bottom_parasite_frame->head_function_index);
  return old_function_stack_bottom;
}


// Pops the bottommost frame off of the stack *stack, and returns a
// pointer to it.
parasite_stack_frame_t* parasite_stack_pop(parasite_stack_t *stack)
{
  parasite_stack_frame_t *old_bottom_parasite_frame = stack->bottom_parasite_frame;
  stack->bottom_parasite_frame = stack->bottom_parasite_frame->parent;
  function_pop(stack);
  return old_bottom_parasite_frame;
}

#endif