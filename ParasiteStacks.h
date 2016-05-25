// Types of functions
typedef enum {
  EMPTY = 0,
  IS_RECURSIVE = 1,
  MAIN = 2,
  SPAWNER = 4,
  HELPER = 6,
  C_FUNCTION = 8
} FunctionType_t;

// Used to size call site status vector 
const int START_FUNCTION_STATUS_VECTOR_SIZE = 4;

// Used to size function scall site stack
const int START_CALL_SITE_STACK_SIZE = 8;

// frame for each function
typedef struct function_frame_t {

  // ID for the function's call site
  CALL_SITE_ID call_site_ID;

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

  // Parent of this function on the same stack 
  struct function_frame_t *parent;

} function_frame_t;

// Type for parasite stack frame
typedef struct parasite_stack_frame_t {

  // Function type
  FunctionType_t func_type;

  // Signature of head function in call stack
  FUN_SG functionSignature;

  // Local continuation span of this function
  TIME local_continuation;

  // Local span of this function
  TIME local_span;

  // Span of the prefix of this function and its child C functions
  TIME prefix_span;

  // Span of the longest spawned child of this function observed so
  // far
  TIME longest_child_span;

  // The span of the continuation is stored in the running_spn
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
