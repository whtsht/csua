#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

typedef union {
    long l_dummy;
    double d_dummy;
    void* p_dummy;
} Cell;

#define CELL_SIZE (sizeof(Cell))
#define DEFAULT_PAGE_SIZE (1024)

typedef struct MemoryPage_tag MemoryPage;
typedef MemoryPage* MemoryPageList;

struct MemoryPage_tag {
    int cell_num;
    int use_cell_num;
    MemoryPageList next;
    Cell cell[1];
};

struct MEM_Storage_tag {
    MemoryPageList page_list;
    int current_page_size;  // number of pages
};

MEM_Storage MEM_open_storage_func(MEM_Controller controller, char* filename,
                                  int line, int page_size) {
    MEM_Storage storage;
    storage = (MEM_Storage)MEM_malloc_func(controller, filename, line,
                                           sizeof(struct MEM_Storage_tag));
    storage->page_list = NULL;
    storage->current_page_size = page_size;
    if (page_size == 0) {
        storage->current_page_size = DEFAULT_PAGE_SIZE;
    }
    return storage;
}

void* MEM_storage_malloc_func(MEM_Controller controller, char* filename,
                              int line, MEM_Storage storage, size_t size) {
    int cellnum = ((size - 1) / CELL_SIZE) + 1;
    //    fprintf(stderr, "cellnum = %d\n", cellnum);
    if (storage->page_list == NULL) { /* do not keep enough memory*/
        //        fprintf(stderr, "new_page_list\n");
        int alloc_num = cellnum > storage->current_page_size
                            ? cellnum
                            : storage->current_page_size;
        MemoryPageList page_list = (MemoryPageList)MEM_malloc_func(
            controller, filename, line,
            sizeof(MemoryPage) + (alloc_num - 1) * CELL_SIZE);
        //        fprintf(stderr, "MemoryPage size: %d, alloc_num: %d, cellsize:
        //        %d\n", (int)sizeof(MemoryPage), (int)alloc_num,
        //        (int)CELL_SIZE); fprintf(stderr, "storage_malloc_size = %d\n",
        //        (int)(sizeof(MemoryPage) + (alloc_num-1) * CELL_SIZE));
        page_list->cell_num = alloc_num;
        page_list->use_cell_num = cellnum;
        page_list->next = NULL;
        storage->page_list = page_list;
        return &page_list->cell[0];
    } else {
        MemoryPageList current_page_list = storage->page_list;
        //        fprintf(stderr, "current_size = %d, use_cell = %d,
        //        required_cell=%d\n", storage->current_page_size,
        //        current_page_list->use_cell_num, cellnum);
        while (current_page_list->next != NULL)
            current_page_list = current_page_list->next;  // move to the last
        if (storage->current_page_size -
                (current_page_list->use_cell_num + cellnum) <
            0) {
            //            fprintf(stderr, "page is NOT enough\n");
            int alloc_num = cellnum > storage->current_page_size
                                ? cellnum
                                : storage->current_page_size;
            MemoryPageList page_list = (MemoryPageList)MEM_malloc_func(
                controller, filename, line,
                sizeof(MemoryPage) + (alloc_num - 1) * CELL_SIZE);
            page_list->cell_num = alloc_num;
            page_list->use_cell_num = alloc_num;
            page_list->next = NULL;
            current_page_list->next = page_list;
            return &page_list->cell[0];
        } else {
            //            fprintf(stderr, "page is enough\n");
            int available_index = current_page_list->use_cell_num;
            current_page_list->use_cell_num += cellnum;
            return &current_page_list->cell[available_index];
        }
    }
    return NULL;
}

void MEM_dispose_func(MEM_Controller controller, MEM_Storage storage) {
    MemoryPage* tmp;
    while (storage->page_list) {
        tmp = storage->page_list->next;
        MEM_free_func(controller, storage->page_list);
        storage->page_list = tmp;
    }
    MEM_free_func(controller, storage);
}
