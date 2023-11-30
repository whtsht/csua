/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   memtest.c
 * Author: hiroaki
 *
 * Created on October 1, 2018, 7:58 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include "../memory/MEM.h"

/*
 *
 */
int main(int argc, char** argv) {
    char* ptr;
    ptr = (char*)MEM_malloc(10);
    for (int i = 0; i < 10; ++i) {
        ptr[i] = 0xaa;
    }
    MEM_dump_memory();
    MEM_free(ptr);
    MEM_dump_memory();

    MEM_Storage storage = MEM_open_storage(0);

    ptr = MEM_storage_malloc(storage, 17);
    for (int i = 0; i < 17; ++i) {
        ptr[i] = 0xee;
    }
    ptr = MEM_storage_malloc(storage, 10);
    for (int i = 0; i < 10; ++i) {
        ptr[i] = 0xef;
    }

    printf("-----------------------\n");
    MEM_dump_memory();
    MEM_dispose(storage);
    MEM_dump_memory();

    return (EXIT_SUCCESS);
}
