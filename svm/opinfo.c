#include <stdint.h>

#include "svm.h"

OpcodeInfo svm_opcode_info[] = {
    {"dummy", "", 0},
    {"push_int", "i", 1},
    {"push_double", "i", 1},
    {"push_stack_int", "i", 1},
    {"push_stack_double", "i", 1},
    {"push_stack_pointer", "", 1},

    {"pop_stack_int", "i", -1},
    {"pop_stack_double", "i", -1},
    {"pop_stack_pointer", "", -1},

    {"push_static_int", "i", 1},
    {"push_static_double", "i", 1},

    {"pop_static_int", "i", -1},
    {"pop_static_double", "i", -1},

    {"add_int", "", -1},
    {"add_double", "", -1},
    {"sub_int", "", -1},
    {"sub_double", "", -1},
    {"mul_int", "", -1},
    {"mul_double", "", -1},
    {"div_int", "", -1},
    {"div_double", "", -1},
    {"mod_int", "", -1},
    {"mod_double", "", -1},
    {"minus_int", "", 0},
    {"minus_double", "", 0},
    {"increment", "", 0},
    {"decrement", "", 0},
    {"cast_int_to_double", "", 0},
    {"cast_double_to_int", "", 0},
    {"eq_int", "", -1},
    {"eq_double", "", -1},
    {"ne_int", "", -1},
    {"ne_double", "", -1},
    {"gt_int", "", -1},
    {"gt_double", "", -1},
    {"ge_int", "", -1},
    {"ge_double", "", -1},
    {"lt_int", "", -1},
    {"lt_double", "", -1},
    {"le_int", "", -1},
    {"le_double", "", -1},
    {"logical_and", "", -1},
    {"logical_or", "", -1},
    {"logical_not", "", -1},
    {"pop", "", -1},
    {"push_function", "i", 0},
    {"invoke", "", -1},
    {"return", "", -1},

};
