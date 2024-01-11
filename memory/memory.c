/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memory.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct MEM_Controller_tag st_default_controller = {
    NULL  // Header
};

MEM_Controller mem_default_controller = &st_default_controller;

typedef union {
    long l_dummy;
    double d_dummy;
    void* p_dummy;
} Align;

#define MARK_SIZE (4)

typedef struct {
    int size;
    char* filename;
    int line;
    Header* prev;
    Header* next;
    uint8_t mark[MARK_SIZE];
} HeaderStruct;

#define ALIGN_SIZE (sizeof(Align)) /* 8 */
#define revalue_up_align(val) ((val) ? (((val)-1) / ALIGN_SIZE + 1) : 0)
#define HEADER_ALIGN_SIZE (revalue_up_align(sizeof(HeaderStruct)))
#define MARK (0xCD)

union Header_tag {
    HeaderStruct s;
    Align u[HEADER_ALIGN_SIZE];
};

static void set_header(Header* header, size_t size, char* filename, int line) {
    header->s.size = size;
    header->s.filename = filename;
    header->s.line = line;
    header->s.next = header->s.prev = NULL;
    memset(header->s.mark, MARK, (char*)&header[1] - (char*)header->s.mark);
}

static void set_footer(Header* header, size_t size) {
    uint8_t* ptr = (uint8_t*)&header[1] + size;
    //    fprintf(stderr, "header: %p, size = %zu, ptr = %p\n", header, size,
    //    ptr);
    memset((void*)ptr, MARK, MARK_SIZE);
}

static void chain_header(MEM_Controller controller, Header* new_header) {
    if (controller->block_header == NULL) {
        //        fprintf(stderr, "chain first\n");
        controller->block_header = new_header;
        return;
    }
    //    fprintf(stderr, "chain from second\n");
    new_header->s.next = controller->block_header;
    controller->block_header->s.prev = new_header;
    controller->block_header = new_header;
}

static void rechain_header(MEM_Controller controller, Header* header) {
    if (header->s.prev) {
        header->s.prev->s.next = header;
    } else {
        controller->block_header = header;
    }
    if (header->s.next) {
        header->s.next->s.prev = header;
    }
}

static void unchain_header(MEM_Controller controller, Header* current_header) {
    if (current_header->s.prev ==
        NULL) {  // controller->block_header points to it
        controller->block_header = current_header->s.next;
        if (current_header->s.next) {
            current_header->s.next->s.prev = NULL;
        }
        return;
    }
    // otherwise
    Header* prev_header = current_header->s.prev;
    prev_header->s.next = current_header->s.next;
    if (current_header->s.next) {
        current_header->s.next->s.prev = prev_header;
    }
}

void MEM_dump_memory_func(MEM_Controller controller) {
    Header* current_header = NULL;
    if ((current_header = controller->block_header) == NULL) {
        fprintf(stderr, "no allocated memory\n");
        return;
    }
    int i;
    uint8_t* ptr;
    while (current_header) {
        uint32_t alloc_size =
            current_header->s.size + sizeof(Header) + MARK_SIZE;
        fprintf(stderr, "-----------------------------------\n");
        fprintf(stderr, "-- size:%d, file:%s, line:%d -- \n",
                current_header->s.size, current_header->s.filename,
                current_header->s.line);
        fprintf(stderr, "-- head:%p, begin:%p --\n", current_header,
                &current_header[1]);
        fprintf(stderr, "-----------------------------------\n");
        ptr = (uint8_t*)current_header;
        for (i = 0; i < alloc_size; ++i, ++ptr) {
            if (i % 16 == 0) fprintf(stderr, "\n");
            fprintf(stderr, "%02x ", *ptr);
        }
        fprintf(stderr, "\n\n");
        current_header = current_header->s.next;
    }
}

void MEM_free_func(MEM_Controller controller, void* bptr) {
    uint8_t* ptr = (uint8_t*)bptr - sizeof(Header);
    //    fprintf(stderr, "free ptr = %p\n", ptr);
    Header* current_header = (Header*)ptr;
    unchain_header(controller, current_header);  // remove from chain
    memset(current_header, 0xcc,
           sizeof(Header) + current_header->s.size + MARK_SIZE);
    free((void*)current_header);
}

void* MEM_malloc_func(MEM_Controller controller, char* filename, int line,
                      size_t size) {
    size_t alloc_size = sizeof(Header) + size + MARK_SIZE;

    Header* header = (Header*)malloc(alloc_size);
    if (header == NULL) {
        fprintf(stderr, "error");
        exit(1);
    }

    memset((void*)header, 0xcc, alloc_size);

    set_header(header, size, filename, line);
    chain_header(controller, header);
    set_footer(header, size);
    return (void*)&header[1];
}

char* MEM_strdup_func(MEM_Controller controller, char* filename, int line,
                      char* src) {
    char* dst = MEM_malloc_func(controller, filename, line, strlen(src) + 1);
    strcpy(dst, src);
    return dst;
}

void* MEM_realloc_func(MEM_Controller controller, char* filename, int line,
                       void* ptr, size_t size) {
    void* new_ptr;
    void* real_ptr;
    Header old_header;
    int old_size;
    size_t alloc_size = sizeof(Header) + size + MARK_SIZE;
    //    fprintf(stderr, "alloc_size = %d\n", (int)alloc_size);

    if (ptr) {
        real_ptr = ptr - sizeof(Header);
        //        fprintf(stderr, "ptr:real_ptr = %p:%p\n", ptr, real_ptr);
        unchain_header(controller, real_ptr);
        old_header = *((Header*)real_ptr);
        old_size = old_header.s.size;
    } else {
        real_ptr = NULL;
        old_size = 0;
    }

    new_ptr = realloc(real_ptr, alloc_size);
    if (new_ptr == NULL) {
        fprintf(stderr, "realloc error");
        if (ptr) {
            free(real_ptr);
        }
        exit(1);
    }

    //    fprintf(stderr, "new_ptr = %p\n", new_ptr);

    if (ptr) {
        *((Header*)new_ptr) = old_header;
        ((Header*)new_ptr)->s.size = size;
        ((Header*)new_ptr)->s.line = line;
        ((Header*)new_ptr)->s.filename = filename;
        rechain_header(controller, new_ptr);
        set_footer(new_ptr, size);
    } else {
        //        memset(new_ptr, 0xcc, alloc_size);
        set_header(new_ptr, size, filename, line);
        chain_header(controller, new_ptr);
        set_footer(new_ptr, size);
    }

    //    fprintf(stderr, "new next = %p\n", ((Header*)new_ptr)->s.next);

    if (size > old_size) {
        memset((char*)new_ptr + old_size + sizeof(Header), 0xcc,
               size - old_size);
    }

    return (void*)&((Header*)new_ptr)[1];
}

void test() {
    int val;
    void* p;

    fprintf(stderr, "test\n");
    fprintf(stderr, "ALIGN_SIZE = %d\n", (int)ALIGN_SIZE);  // 8
    fprintf(stderr, "HeaderStruct size = %d\n",
            (int)sizeof(HeaderStruct));                                   // 48
    fprintf(stderr, "HEADER_ALIGN_SIZE = %d\n", (int)HEADER_ALIGN_SIZE);  // 6
    fprintf(stderr, "u size = %d\n", (int)(sizeof(Align) * HEADER_ALIGN_SIZE));
    fprintf(stderr, "long size = %d\n", (int)sizeof(long));
    fprintf(stderr, "double size = %d\n", (int)sizeof(double));
    fprintf(stderr, "void*  size = %d\n", (int)sizeof(p));
    fprintf(stderr, "int    size = %d\n", (int)sizeof(int));

    val = sizeof(HeaderStruct);
    val = 0;
    if (val) {
        fprintf(stderr, "val = %d\n", val);
        fprintf(stderr, "val2= %d\n", (val - 1) / 8);

    } else {
        fprintf(stderr, "boooo\n");
    }
}
