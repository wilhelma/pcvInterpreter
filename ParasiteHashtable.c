#include "Parasite_hashtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Method implementations
 */

// Starting capacity of the hash table is 2^2 entries.
/* static */ const int START_HASHTABLE_LOG_CAPACITY = 2;

// Threshold fraction of table size that can be in the linked list.
/* static */ const int MAX_FRACTION_DENOM_IN_LINKED_LIST = 4;

int MIN_HASHTABLE_CAPACITY = 1;

parasite_hashtable_list_node_t *linked_list_free_list = NULL;

// Return true if this entry is empty, false otherwise.
bool hashtable_entry_is_empty(const parasite_hashtable_entry_t *entry) {
  return (0 == entry->initialized);
}

// Create an empty hashtable entry
static void make_empty_hashtable_entry(parasite_hashtable_entry_t *entry) {
  entry->initialized = 0;
}


// Allocate an empty hash table with 2^log_capacity entries
static parasite_hashtable_t* parasite_hashtable_alloc(int log_capacity) {

  assert(log_capacity >= START_HASHTABLE_LOG_CAPACITY);
  size_t capacity = 1 << log_capacity;
  parasite_hashtable_t *table =
    (parasite_hashtable_t*)malloc(sizeof(parasite_hashtable_t)
			    + (capacity * sizeof(parasite_hashtable_entry_t)));

  int *populated_entries = (int*)malloc(sizeof(int) * capacity);

  table->log_capacity = log_capacity;
  table->list_size = 0;
  table->table_size = 0;
  table->head = NULL;
  table->tail = NULL;
  table->populated = populated_entries;

  return table;
}


// Create a new, empty hashtable.  Returns a pointer to the hashtable
// created.
parasite_hashtable_t* parasite_hashtable_create() {
  parasite_hashtable_t *table = parasite_hashtable_alloc(START_HASHTABLE_LOG_CAPACITY);
  for (size_t i = 0; i < (1 << START_HASHTABLE_LOG_CAPACITY); ++i) {
    make_empty_hashtable_entry(&(table->entries[i]));
  }
  return table;
}


static inline
int can_override_entry(parasite_hashtable_entry_t *entry, uintptr_t new_call_site_id) {
  // used to be this:
  // entry->call_site_id == new_call_site_id && entry->height == new_height
  return (entry->call_site_id == new_call_site_id);
}


static inline
void combine_entries(parasite_hashtable_entry_t *entry,
                     const parasite_hashtable_entry_t *entry_add) {
  entry->local_work += entry_add->local_work;
  entry->local_span += entry_add->local_span;
  entry->local_count += entry_add->local_count;
  entry->work += entry_add->work;
  entry->span += entry_add->span;
  entry->count += entry_add->count;
  entry->top_work += entry_add->top_work;
  entry->top_span += entry_add->top_span;
  entry->top_count += entry_add->top_count;
}


// Return a hashtable with the contents of table and more capacity.
static parasite_hashtable_t* increase_hashtable_table_capacity(const parasite_hashtable_t *table) {

  int new_log_capacity;
  if ((1 << table->log_capacity) < MIN_CAPACITY) {
    uint32_t x = MIN_CAPACITY - 1;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    new_log_capacity = __builtin_ctz(x + 1);
  } else {

    fprintf(stderr, "ERROR: problem with call site capacity over minimum \n");
    new_log_capacity = table->log_capacity + 1;
  }

  parasite_hashtable_t *new_table;

  new_tab = parasite_hashtable_alloc(new_log_capacity);
  size_t i = 0;

  for (i = 0; i < (1 << table->log_capacity); ++i) {
    new_table->entries[i] = table->entries[i];
  }

  for ( ; i < (1 << new_table->log_capacity); ++i) {
    make_empty_hashtable_entry(&(new_table->entries[i]));
  }

  for (i = 0; i < table->table_size; ++i) {
    new_table->populated[i] = table->populated[i];
  }

  new_table->table_size = table->table_size;

  new_table->list_size = table->list_size;
  new_table->head = table->head;
  new_table->tail = table->tail;

  return new_table;
}


// Add entry to table, resizing table if necessary.  Returns a pointer to
// the entry if it can find a place to store it, NULL otherwise.
/* static */ __attribute__((always_inline)) parasite_hashtable_entry_t*
get_parasite_hashtable_entry_at_index(uint32_t index, parasite_hashtable_t **table) {
  if (index >= (1 << (*table)->log_capacity)) {
    parasite_hashtable_t *new_tab = increase_hashtable_table_capacity(*table);

    assert(new_table);
    assert(new_table->head == (*table)->head);
    assert(new_table->tail == (*table)->tail);
    (*table)->head = NULL;
    (*table)->tail = NULL;

    free((*table)->populated);
    free(*table);
    *table= new_table;
  }

  return &((*table)->entries[index]);
}


static inline
void flush_parasite_hashtable_list(parasite_hashtable_t **table) {

  // Flush list into table
  parasite_hashtable_linked_list_node_t *lst_entry = (*table)->head;
  int entries_added = 0;

  while (NULL != lst_entry) {
    parasite_hashtable_entry_t *entry = &(lst_entry->entry);

    if (lst_entry == (*table)->tail) {
      assert(entries_added == (*table)->list_size - 1);
      assert(lst_entry->next == NULL);
    }

    parasite_hashtable_entry_t *tab_entry;

    /* tab_entry = get_parasite_hashtable_entry(entry->call_site_id, table); */
    tab_entry = get_parasite_hashtable_entry_at_index(lst_entry->index, table);
    assert(NULL != tab_entry);
    assert(empty_hashtable_entry_p(tab_entry) || can_override_entry(tab_entry, entry->call_site_id));

    if (empty_hashtable_entry_p(tab_entry)) {
      // the compiler will do a struct copy
      *tab_entry = *entry;
      tab_entry->initialized = 1;
      /* (*table)->populated[(*table)->table_size] = hashtable_index(entry->call_site_id); */
      (*table)->populated[(*table)->table_size] = lst_entry->index;
      ++(*table)->table_size;
    } else {
      combine_entries(tab_entry, entry);
    }

    entries_added++;
    parasite_hashtable_linked_list_node_t *next_lst_entry = lst_entry->next;
    // free(lst_entry);
    lst_entry = next_lst_entry;
  }

  if (NULL != (*table)->head) {
    assert(NULL != (*table)->tail);
    (*table)->tail->next = ll_free_list;
    ll_free_list = (*table)->head;
  }

  (*table)->head = NULL;
  (*table)->tail = NULL;
  (*table)->list_size = 0;
}

void flush_parasite_hashtable(parasite_hashtable_t **table) {
  if (NULL != (*table)->head)
    flush_parasite_hashtable_list(table);
  else
    assert((*table)->list_size == 0);
}


// Add the given parasite_hashtable_entry_t data to **tab.  Returns true if
// data was suhashtableessfully added, false otherwise.
__attribute__((always_inline))
bool add_to_parasite_hashtable(parasite_hashtable_t **table,
                         /* int32_t depth, bool is_top_level, */
                         /* InstanceType_t inst_type, */
                         bool is_top_fn,
                         uint32_t index,
                         CALL_SITE_ID call_site_id,
                         uint64_t work, uint64_t span,
                         uint64_t local_work, uint64_t local_span) {
  
  if (index >= (1 << (*table)->log_capacity) &&
      /* (1 << (*table)->log_capacity) < MIN_CAPACITY && */
      (*table)->list_size < MIN_CAPACITY * MAX_FRACTION_DENOM_IN_LINKED_LIST) {
    // If the table_size + list_size is sufficiently small, add entry
    // to linked list.
    parasite_hashtable_linked_list_node_t *lst_entry;
    if (NULL != ll_free_list) {
      lst_entry = ll_free_list;
      ll_free_list = ll_free_list->next;
    } else {
      lst_entry = (parasite_hashtable_linked_list_node_t*)malloc(sizeof(parasite_hashtable_linked_list_node_t));
    }

    lst_entry->index = index;

    /* lst_entry->entry.is_recursive = (0 != (RECURSIVE & inst_type)); */
    lst_entry->entry.call_site_id = call_site_id;
    lst_entry->entry.work = work;
    lst_entry->entry.span = span;
    lst_entry->entry.count = 1; /* (0 != (RECORD & inst_type)); */
    /* if (TOP & inst_type) { */
    if (is_top_fn) {
      lst_entry->entry.top_work = work;
      lst_entry->entry.top_span = span;
      assert(0 != work);
      lst_entry->entry.top_count = 1;
    } else {
      lst_entry->entry.top_work = 0;
      lst_entry->entry.top_span = 0;
      lst_entry->entry.top_count = 0;
    }      
    lst_entry->entry.local_work = local_work;
    lst_entry->entry.local_span = local_span;
    lst_entry->entry.local_count = 1;
    lst_entry->next = NULL;

    if (NULL == (*table)->tail) {
      (*table)->tail = lst_entry;
      assert(NULL == (*table)->head);
      (*table)->head = lst_entry;
    } else {
      (*table)->tail->next = lst_entry;
      (*table)->tail = lst_entry;
    }

    // Increment list size
    ++(*table)->list_size;

  } else {
    
    if ((*table)->list_size > 0) {
      assert((*table)->head != NULL);
      flush_parasite_hashtable_list(table);
    }

    // Otherwise, add it to the table directly
    /* parasite_hashtable_entry_t *entry = get_parasite_hashtable_entry(call_site_id, table); */
    parasite_hashtable_entry_t *entry = get_parasite_hashtable_entry_at_index(index, table);
    assert(NULL != entry);
    assert(empty_hashtable_entry_p(entry) || can_override_entry(entry, call_site_id));
  
    if (empty_hashtable_entry_p(entry)) {
      /* entry->is_recursive = (0 != (RECURSIVE & inst_type)); */

      entry->call_site_id = call_site_id;

      entry->initialized = 1;
      entry->work = work;
      entry->span = span;
      entry->count = 1; /* (0 != (RECORD & inst_type)); */
      /* if (TOP & inst_type) { */
      if (is_top_fn) {
        entry->top_work = work;
        entry->top_span = span;
        entry->top_count = 1;
      } else {
        entry->top_work = 0;
        entry->top_span = 0;
        entry->top_count = 0;
      }
      entry->local_work = local_work;
      entry->local_span = local_span;
      entry->local_count = 1;
      /* (*table)->populated[ (*table)->table_size ] = hashtable_index(call_site_id); */
      (*table)->populated[ (*table)->table_size ] = index;
      ++(*table)->table_size;
    } else {
      entry->local_work += local_work;
      entry->local_span += local_span;
      entry->local_count += 1;
      entry->work += work;
      entry->span += span;
      entry->count += 1; /* (0 != (RECORD & inst_type)); */
      /* if (TOP & inst_type) { */
      if (is_top_fn) {
        entry->top_work += work;
        entry->top_span += span;
        entry->top_count += 1;
      }
    }
  }

  return true;
}

// Add the given parasite_hashtable_entry_t data to **tab.  Returns true if
// data was suhashtableessfully added, false otherwise.
__attribute__((always_inline))
bool add_local_to_parasite_hashtable(parasite_hashtable_t **table,
                               uint32_t index,
                               CALL_SITE_ID call_site_id,
                               uint64_t local_work, uint64_t local_span) {

  if (index >= (1 << (*table)->log_capacity) &&
      /* (1 << (*table)->log_capacity) < MIN_CAPACITY && */
      (*table)->list_size < MIN_CAPACITY * MAX_FRACTION_DENOM_IN_LINKED_LIST) {
    // If the table_size + list_size is sufficiently small, add entry
    // to linked list.
    parasite_hashtable_linked_list_node_t *lst_entry;
    if (NULL != ll_free_list) {
      lst_entry = ll_free_list;
      ll_free_list = ll_free_list->next;
    } else {
      lst_entry = (parasite_hashtable_linked_list_node_t*)malloc(sizeof(parasite_hashtable_linked_list_node_t));
    }

    lst_entry->index = index;
    lst_entry->entry.call_site_id = call_site_id;
    lst_entry->entry.work = 0;
    lst_entry->entry.span = 0;
    lst_entry->entry.count = 0;
    lst_entry->entry.top_work = 0;
    lst_entry->entry.top_span = 0;
    lst_entry->entry.top_count = 0;
    lst_entry->entry.local_work = local_work;
    lst_entry->entry.local_span = local_span;
    lst_entry->entry.local_count = 1;
    lst_entry->next = NULL;

    if (NULL == (*table)->tail) {
      (*table)->tail = lst_entry;
      assert(NULL == (*table)->head);
      (*table)->head = lst_entry;
    } else {
      (*table)->tail->next = lst_entry;
      (*table)->tail = lst_entry;
    }

    // Increment list size
    ++(*table)->list_size;

  } else {
    
    if ((*table)->list_size > 0) {
      assert((*table)->head != NULL);
      flush_parasite_hashtable_list(table);
    }

    // Otherwise, add it to the table directly
    /* parasite_hashtable_entry_t *entry = get_parasite_hashtable_entry(call_site_id, table); */
    parasite_hashtable_entry_t *entry = get_parasite_hashtable_entry_at_index(index, table);
    assert(NULL != entry);
    assert(empty_hashtable_entry_p(entry) || can_override_entry(entry, call_site_id));
  
    if (empty_hashtable_entry_p(entry)) {

      entry->call_site_id = call_site_id;

      entry->initialized = 1;
      entry->work = 0;
      entry->span = 0;
      entry->count = 0;
      entry->top_work = 0;
      entry->top_span = 0;
      entry->top_count = 0;
      entry->local_work = local_work;
      entry->local_span = local_span;
      entry->local_count = 1;
      (*table)->populated[ (*table)->table_size ] = index;
      ++(*table)->table_size;
    } else {
      assert(call_site_id == entry->call_site_id);
      entry->local_work += local_work;
      entry->local_span += local_span;
      entry->local_count += 1;
    }
  }

  return true;
}

// Add the parasite_hashtable **right into the parasite_hashtable **left.  The
// result will appear in **left, and **right might be modified in the
// process.
__attribute__((always_inline))
parasite_hashtable_t* add_parasite_hashtables(parasite_hashtable_t **left, parasite_hashtable_t **right) {

  /* fprintf(stderr, "add_parasite_hashtables(%p, %p)\n", left, right); */

  // Make sure that *left is at least as large as *right.
  if ((*right)->log_capacity > (*left)->log_capacity) {
    parasite_hashtable_t *tmp = *left;
    *left = *right;
    *right = tmp;
  }

  /* fprintf(stderr, "\tleft list_size = %d, right list_size = %d\n", */
  /* 	  (*left)->list_size, (*right)->list_size); */

  if (NULL != (*left)->tail) {
    (*left)->tail->next = (*right)->head;
  } else {
    assert(NULL == (*left)->head);
    (*left)->head = (*right)->head;
    // XXX: Why not just do this?  Does it matter if both are NULL?
    /* (*left)->tail = (*right)->tail; */
  }
  (*left)->list_size += (*right)->list_size;
  if (NULL != (*right)->tail) {
    (*left)->tail = (*right)->tail;
  }
  (*right)->head = NULL;
  (*right)->tail = NULL;
  (*right)->list_size = 0;

  /* fprintf(stderr, "list_size = %d, table_size = %d, log_capacity = %d\n", */
  /* 	  (*left)->list_size, (*left)->table_size, (*left)->log_capacity); */

  if ((*left)->list_size >= MIN_CAPACITY * MAX_FRACTION_DENOM_IN_LINKED_LIST) {
    flush_parasite_hashtable_list(left);
  }

  parasite_hashtable_entry_t *l_entry, *r_entry;
  for (size_t i = 0; i < (*right)->table_size; ++i) {

    r_entry = &((*right)->entries[ (*right)->populated[i] ]);
    assert(!empty_hashtable_entry_p(r_entry));

    l_entry = &((*left)->entries[ (*right)->populated[i] ]);
    assert(NULL != l_entry);
    assert(empty_hashtable_entry_p(l_entry) || can_override_entry(l_entry, r_entry->call_site_id));

    if (empty_hashtable_entry_p(l_entry)) {
      // let the compiler do the struct copy
      *l_entry = *r_entry;
      (*left)->populated[ (*left)->table_size ] = (*right)->populated[i];
      ++(*left)->table_size;
    } else {
      combine_entries(l_entry, r_entry);
    }

  }

  return *left;
}

// Clear all entries in tab.
void clear_parasite_hashtable(parasite_hashtable_t *table) {
  // Clear the linked list
  /* parasite_hashtable_linked_list_node_t *lst_entry = table->head; */
  /* while (NULL != lst_entry) { */
  /*   parasite_hashtable_linked_list_node_t *next_lst_entry = lst_entry->next; */
  /*   free(lst_entry); */
  /*   lst_entry = next_lst_entry; */
  /* } */

  if (NULL != table->head) {
    assert(NULL != table->tail);
    table->tail->next = ll_free_list;
    ll_free_list = table->head;
  }

  table->head = NULL;
  table->tail = NULL;
  table->list_size = 0;

  // Clear the table
  for (size_t i = 0; i < table->table_size; ++i) {
    make_empty_hashtable_entry(&(table->entries[ table->populated[i] ]));
  }
  table->table_size = 0;
}

// Free a table.
void free_parasite_hashtable(parasite_hashtable_t *table) {
  // Clear the linked list
  /* parasite_hashtable_linked_list_node_t *lst_entry = table->head; */
  /* while (NULL != lst_entry) { */
  /*   parasite_hashtable_linked_list_node_t *next_lst_entry = lst_entry->next; */
  /*   free(lst_entry); */
  /*   lst_entry = next_lst_entry; */
  /* } */
  if (NULL != table->head) {
    assert(NULL != table->tail);
    table->tail->next = ll_free_list;
    ll_free_list = table->head;
  }
  free(table->populated);
  free(table);
}

bool parasite_hashtable_is_empty(const parasite_hashtable_t *table) {
  return table->table_size == 0 && table->list_size == 0;
}
