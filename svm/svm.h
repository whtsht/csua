
#ifndef _SVM_H_
#define _SVM_H_
#include <stdint.h>

typedef struct SVM_VirtualMachine_tag SVM_VirtualMachine;

typedef enum
{
    SVM_PUSH_INT = 1,
    SVM_PUSH_DOUBLE,
    SVM_PUSH_STACK_INT,
    SVM_PUSH_STACK_DOUBLE,
    SVM_PUSH_STACK_PT, //
    SVM_POP_STACK_INT,
    SVM_POP_STACK_DOUBLE,
    SVM_POP_STACK_PT, //
    SVM_PUSH_STATIC_INT,
    SVM_PUSH_STATIC_DOUBLE,
    SVM_POP_STATIC_INT,
    SVM_POP_STATIC_DOUBLE,
    SVM_ADD_INT,
    SVM_ADD_DOUBLE,
    SVM_SUB_INT,
    SVM_SUB_DOUBLE,
    SVM_MUL_INT,
    SVM_MUL_DOUBLE,
    SVM_DIV_INT,
    SVM_DIV_DOUBLE,
    SVM_MOD_INT,
    SVM_MOD_DOUBLE,
    SVM_MINUS_INT,
    SVM_MINUS_DOUBLE,
    SVM_INCREMENT,
    SVM_DECREMENT,
    SVM_CAST_INT_TO_DOUBLE,
    SVM_CAST_DOUBLE_TO_INT,
    SVM_EQ_INT,
    SVM_EQ_DOUBLE,
    SVM_NE_INT,
    SVM_NE_DOUBLE,
    SVM_GT_INT,
    SVM_GT_DOUBLE,
    SVM_GE_INT,
    SVM_GE_DOUBLE,
    SVM_LT_INT,
    SVM_LT_DOUBLE,
    SVM_LE_INT,
    SVM_LE_DOUBLE,
    SVM_LOGICAL_AND,
    SVM_LOGICAL_OR,
    SVM_LOGICAL_NOT,
    SVM_POP,
    SVM_PUSH_FUNCTION,
    SVM_INVOKE,
    SVM_RETURN
} SVM_Opcode;

typedef enum
{
    SVM_INT = 1,
    SVM_DOUBLE,
} SVM_ConstantType;

typedef struct
{
    SVM_ConstantType type;
    union
    {
        int c_int;
        double c_double;
    } u;
} SVM_Constant;

typedef union
{
    int ival;
    double dval;
} SVM_Value;

typedef struct
{
    char *opname;
    char *parameter;
    char s_size;
} OpcodeInfo;

typedef enum
{
    NATIVE_FUNCTION,
    CSUA_FUNCTION
} FunctionType;

typedef SVM_Value (*SVM_NativeFunction)(SVM_VirtualMachine *svm,
                                        SVM_Value *values, int arg_count);

typedef struct
{
    FunctionType f_type;
    char *name;
    int arg_count;
    union
    {
        SVM_NativeFunction n_func;
    } u;
} SVM_Function;

struct SVM_VirtualMachine_tag
{
    uint32_t constant_pool_count;
    SVM_Constant *constant_pool;
    uint32_t global_variable_count;
    SVM_Value *global_variables;
    uint8_t *global_variable_types;
    uint32_t code_size;
    uint8_t *code;
    uint32_t function_count;
    SVM_Function *functions;
    uint32_t stack_size;
    uint8_t *stack_value_type;
    SVM_Value *stack;
    uint32_t pt_stack_size; //
    uint8_t *pt_stack;      //
    size_t pt_stack_count;  //
    uint32_t pc;
    uint32_t sp;
};

extern OpcodeInfo svm_opcode_info[];

/* svm.c */
void svm_add_native_function(SVM_VirtualMachine *svm,
                             SVM_NativeFunction native_f, char *name,
                             int arg_count);

/* native.c */
void add_native_functions(SVM_VirtualMachine *svm);
#endif