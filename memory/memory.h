/*
 * File:   mem.h
 * Author: hiroaki
 *
 * Created on December 22, 2016, 10:04 AM
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "MEM.h"

typedef union Header_tag Header;

struct MEM_Controller_tag {
    Header *block_header;
};

#endif /* MEMORY_H */
