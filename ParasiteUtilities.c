#define _GNU_SOURCE

#include "ParasiteUtilities.h"

const char *FunctionType_str[FUNCTIONTYPE_END] = { "empty", "recursive", "main", "INVALID", "cilk", "INVALID", "helper", "INVALID", "c" };

mapping_list_t maps = { .head = NULL, .tail = NULL };

// Ensure that this tool is run serially
void ensure_serial_tool(void) {
  // assert(1 == __cilkrts_get_nworkers());
  fprintf(stderr, "Forcing CILK_NWORKERS=1.\n");
  char *e = getenv("CILK_NWORKERS");
  if (!e || 0!=strcmp(e, "1")) {
    // fprintf(err_io, "Setting CILK_NWORKERS to be 1\n");
    if( setenv("CILK_NWORKERS", "1", 1) ) {
      fprintf(stderr, "Error setting CILK_NWORKERS to be 1\n");
      exit(1);
    }
  }
}

// The user of this function should free the char * returned after you are
// done with the info.
char* get_info_on_inst_addr(uint64_t addr, int *line_no, char **file) {

  mapping_list_el_t *map_lst_el = maps.head;
  while (NULL != map_lst_el) {
    mapping_t *map = &(map_lst_el->map);
    if (map->low <= addr && addr < map->high) {
      unsigned long off = addr - map->low;
      const char *path = map->path;
      bool is_so = strcmp(".so", path+strlen(path)-3) == 0;
      char *command;
      if (is_so) {
        asprintf(&command, "echo %lx | addr2line -e %s", off, path);
      } else {
        asprintf(&command, "echo %lx | addr2line -e %s", addr, path);
      }
      FILE *afile = popen(command, "r");
      char *line = NULL;
      if (afile) {
        size_t linelen = -1; // max size
        if (getline(&line, &linelen, afile)>=0) {
          *file = strtok(line, ":");
          const char *lno = strtok(NULL, ":");
          *line_no = atoi(lno);
        }
        // if (line) free(line);
        pclose(afile);
      }
      free(command);
      return line;
    }
    map_lst_el = map_lst_el->next;
  }

  fprintf(stderr, "address %lx is not in range\n", addr);
  return NULL;
}

void print_addr(uintptr_t a) {
  uintptr_t ai = a;
  /* if (1) printf(" PC= %lx\n", a); */
  mapping_list_el_t *map_lst_el = maps.head;
  while (NULL != map_lst_el) {
    mapping_t *map = &(map_lst_el->map);
    if (0) printf("Comparing %lx to %lx:%lx\n", ai, map->low, map->high);
    if (map->low <= ai && ai < map->high) {
      uintptr_t off = ai-map->low;
      const char *path = map->path;
      /* if (1) printf("%lx is offset 0x%lx in %s\n", a, off, path); */
      bool is_so = strcmp(".so", path+strlen(path)-3) == 0;
      if (0) {if (is_so) printf(" is so\n"); else printf(" not so\n");}
      char *command;
      if (is_so) {
	asprintf(&command, "echo %lx | addr2line -e %s", off, path);
      } else {
	asprintf(&command, "echo %lx | addr2line -e %s", ai, path);
      }
      /* if (debug_level>1) printf("Doing system(\"%s\");\n", command); */
      system(command);
      free(command);
      return;
    }
    map_lst_el = map_lst_el->next;
  }
  printf("address %lx is not in range\n", a);
}
