#ifndef _PARASITE_HASHTABLE_H_
#define _PARASITE_HASHTABLE_H_

#include <stdbool.h>
#include "ParasiteUtilities.h"

/**
 * Data structures
 */

// Structure for a hashtable entry
typedef struct {

  // Store whether this entry is initialized
  bool is_initialized;

  // Function type.  Least-significant bit indicates whether the
  // function is recursive.
  FunctionType_t func_type;

 // ID for the function's call site
  //CALL_SITE_ID call_site_ID;
  int call_site_ID;

  // Total number of invocations of call_site_ID
  int local_call_site_invocation_count;

  // Local work associated with call_site_ID
  double local_work;

  // Local span associated with call_site_ID
  double local_span;

  // Number of invocations of call_site_ID, excluding recursive instances.
  int count;

  // Number of top-level invocations of call_site_ID.
  int top_count;

  // Work associated with call_site_ID, excluding recursive instances
  double work;
  // Span associated with call_site_ID, excluding recursive instances
  double span;

  // Work associated with top-level invocations of call_site_ID
  double top_work;
  
  // Span associated with top-level invocations of call_site_ID
  double top_span;

} parasite_hashtable_entry_t;

// Structure for making a linked list of parasite_hashtable entries
typedef struct parasite_hashtable_linked_list_node_t {

  // Index in table for this entry
  int index;

  // Pointer to next entry in table
  struct parasite_hashtable_linked_list_node_t* next;

  // Hashtable entry data
  parasite_hashtable_entry_t entry;

} parasite_hashtable_linked_list_node_t;

// Structure for the hashtable
typedef struct {

  // Logarithm (?) of capacity of hash table
  int log_capacity;

  // Number of elements in list
  int list_size;

  // Number of elements in table
  int table_size;

  // Linked list of entries to add to hashtable
  parasite_hashtable_linked_list_node_t *head;
  parasite_hashtable_linked_list_node_t *tail;

  // Array storing indices of entries[] that are nonzero
  int *populated;

  // Entries of the hash table
  parasite_hashtable_entry_t entries[0];

} parasite_hashtable_t;

extern parasite_hashtable_linked_list_node_t *linked_list_free_list;  

bool hashtable_entry_is_empty(const parasite_hashtable_entry_t *entry);

parasite_hashtable_t* parasite_hashtable_create();
parasite_hashtable_t* add_parasite_hashtables(parasite_hashtable_t **left, parasite_hashtable_t **right);

void clear_parasite_hashtable(parasite_hashtable_t *table);
void flush_parasite_hashtable(parasite_hashtable_t **table);

bool add_to_parasite_hashtable(parasite_hashtable_t **table,
                         bool is_top_fn,
                         int index,
                         int call_site_id,
                         // CALL_SITE_ID call_site_id,
                         double work, double span,
                         double local_work, double local_span);

bool add_local_to_parasite_hashtable(parasite_hashtable_t **table,
                               int index,
                               int call_site_id,
                               // CALL_SITE_ID call_site_id,
                               double local_work, double local_span);

void free_parasite_hashtable(parasite_hashtable_t *table); 

bool parasite_hashtable_is_empty(const parasite_hashtable_t *table);

#endif