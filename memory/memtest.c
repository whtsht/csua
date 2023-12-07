#include <stdio.h>

#include "MEM.h"
// #include "../info.h"

struct st {
    MEM_Storage head;
};

int main(void) {
    char *ptr, *ptr2;
    //    LocalInfo *l_info;
    int i;
    test();

    ptr = (char*)MEM_malloc(10);
    for (i = 0; i < 10; ++i) {
        ptr[i] = 0xaa;
    }
    MEM_dump_memory();
    MEM_free(ptr);

    //    ptr2 = (char*)MEM_malloc(20);
    ptr2 = NULL;

    MEM_dump_memory();
    fprintf(stderr, "do realloc\n");
    ptr2 = MEM_realloc(ptr2, 30);
    for (i = 0; i < 20; ++i) {
        ptr2[i] = 0xab;
    }
    MEM_dump_memory();
    //    MEM_free(ptr);
    //    fprintf(stderr, "--dump--\n");
    //    MEM_dump_memory();
    MEM_free(ptr2);
    fprintf(stderr, "--dump--\n");
    MEM_dump_memory();
    /*
    l_info = (LocalInfo*)MEM_malloc(sizeof(LocalInfo));
    l_info->line_number = 0xff;
    MEM_dump_memory();
    MEM_free(l_info);
    */

    fprintf(stderr, "------------------------------------>>>>>>\n");

    MEM_Storage storage = MEM_open_storage(0);

    struct st* pt = (struct st*)MEM_storage_malloc(storage, sizeof(struct st));
    pt->head = MEM_open_storage(0);
    fprintf(stderr, "ptr addr = %p\n", pt->head);

    ptr = MEM_storage_malloc(storage, 10);
    for (i = 0; i < 10; ++i) {
        ptr[i] = 0xaa;
    }

    ptr2 = MEM_storage_malloc(storage, 10);
    for (i = 0; i < 10; ++i) {
        ptr2[i] = 0xbb;
    }

    fprintf(stderr, "------------------------------------>>>>>>\n");
    char* src = "Hello World";
    char* copy = MEM_strdup(src);
    printf("copy = %s\n", copy);
    MEM_dump_memory();
    MEM_free(copy);

    MEM_dispose(pt->head);
    MEM_dispose(storage);
    //	fprintf(stderr, "addr = %p\n", pt->head);

    MEM_dump_memory();

    return 0;
}
