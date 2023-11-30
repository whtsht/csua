#ifndef _MEM_H_
#define _MEM_H_

#include <stddef.h>

typedef struct MEM_Controller_tag* MEM_Controller;
typedef struct MEM_Storage_tag* MEM_Storage;

extern MEM_Controller mem_default_controller;

/* For Open Interface */
/* Malloc */
#define MEM_malloc(size) \
    MEM_malloc_func(mem_default_controller, __FILE__, __LINE__, size)
#define MEM_strdup(str) \
    MEM_strdup_func(mem_default_controller, __FILE__, __LINE__, str)
#define MEM_dump_memory() MEM_dump_memory_func(mem_default_controller)
#define MEM_free(ptr) MEM_free_func(mem_default_controller, ptr)
#define MEM_realloc(ptr, size) \
    MEM_realloc_func(mem_default_controller, __FILE__, __LINE__, ptr, size)

/* Storage */
#define MEM_open_storage(page_size) \
    MEM_open_storage_func(mem_default_controller, __FILE__, __LINE__, page_size)
#define MEM_storage_malloc(storage, size)                               \
    MEM_storage_malloc_func(mem_default_controller, __FILE__, __LINE__, \
                            storage, size)
#define MEM_dispose(storage) MEM_dispose_func(mem_default_controller, storage)

void test();

/* For Inner functions */
/* Malloc */
void* MEM_malloc_func(MEM_Controller controller, char* filename, int line,
                      size_t size);
void* MEM_realloc_func(MEM_Controller controller, char* filename, int line,
                       void* ptr, size_t size);
void MEM_dump_memory_func(MEM_Controller controller);
void MEM_free_func(MEM_Controller controller, void* bptr);
char* MEM_strdup_func(MEM_Controller controller, char* filename, int line,
                      char* src);

/* Storage */
MEM_Storage MEM_open_storage_func(MEM_Controller controller, char* filename,
                                  int line, int page_size);
void* MEM_storage_malloc_func(MEM_Controller controller, char* filename,
                              int line, MEM_Storage storage, size_t size);

void MEM_dispose_func(MEM_Controller controller, MEM_Storage storage);

#endif
