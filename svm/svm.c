#include "svm.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../memory/MEM.h"

static int read_int(uint8_t **p)
{
    uint8_t v1 = **p;
    (*p)++;
    uint8_t v2 = **p;
    (*p)++;
    uint8_t v3 = **p;
    (*p)++;
    uint8_t v4 = **p;
    (*p)++;
    int v = (int)(v1 << 24 | v2 << 16 | v3 << 8 | v4);
    return v;
}

static double read_double(uint8_t **p)
{
    uint8_t v1 = **p;
    (*p)++;
    uint8_t v2 = **p;
    (*p)++;
    uint8_t v3 = **p;
    (*p)++;
    uint8_t v4 = **p;
    (*p)++;
    uint8_t v5 = **p;
    (*p)++;
    uint8_t v6 = **p;
    (*p)++;
    uint8_t v7 = **p;
    (*p)++;
    uint8_t v8 = **p;
    (*p)++;

    long lv = (long)((long)v1 << 56 | (long)v2 << 48 | (long)v3 << 40 |
                     (long)v4 << 32 | (long)v5 << 24 | (long)v6 << 16 |
                     (long)v7 << 8 | (long)v8);
    double *pdv = (double *)&lv;
    return *pdv;
}

static uint8_t read_byte(uint8_t **p)
{
    uint8_t v = **p;
    (*p)++;
    return v;
}

static void parse_header(uint8_t **p)
{
    for (int i = 0; i < 8; ++i)
    {
        //        printf("%c ", (char)(**p));
        (*p)++;
    }
    //    printf("\n");
}

typedef struct
{
    char s_buf[128];
    char row_buf[32];
    int s_index;
    int r_index;

} DInfo;

static void dump(DInfo *info)
{
    fprintf(stderr, "%s", info->s_buf);
    fprintf(stderr, "   %s\n", info->row_buf);
    info->s_index = 0;
    info->r_index = 0;
}

static void add_opname(DInfo *info, const char *str)
{
    int len = strlen(str);
    strncpy(&info->s_buf[info->s_index], str, len);
    info->s_index += len;
    for (int i = 0; i < (20 - len); ++i, ++info->s_index)
    {
        info->s_buf[info->s_index] = ' ';
    }
    info->s_buf[info->s_index] = 0;
}

static void add_padding(DInfo *info)
{
    for (int i = 0; i < 4; ++i, ++info->s_index)
        info->s_buf[info->s_index] = ' ';
}

static void add_uint16(DInfo *info, const uint16_t iv)
{
    char buf[6] = {};
    sprintf(buf, "%04x", iv);
    strncpy(&info->s_buf[info->s_index], buf, 4);
}

static void add_rowcode(DInfo *info, uint8_t op)
{
    //    printf("add row %02x\n", op);
    sprintf(&info->row_buf[info->r_index], "%02x", op);
    info->r_index += 2;
    info->row_buf[info->r_index] = 0;
}

static void disasm(SVM_VirtualMachine *svm)
{
    printf("-- constant pool --\n");
    printf("constant_count = %d\n", svm->constant_pool_count);
    for (int i = 0; i < svm->constant_pool_count; ++i)
    {
        printf("constant[%d] = ", i);
        switch (svm->constant_pool[i].type)
        {
        case SVM_INT:
        {
            printf("%d\n", svm->constant_pool[i].u.c_int);
            break;
        }
        case SVM_DOUBLE:
        {
            printf("%f\n", svm->constant_pool[i].u.c_double);
            break;
        }
        default:
        {
            exit(1);
        }
        }
    }

    printf("\n-- variables --\n");
    printf("variable_count = %d\n", (int)svm->global_variable_count);
    for (int i = 0; i < svm->global_variable_count; ++i)
    {
        printf("v[%d]: ", i);
        switch (svm->global_variable_types[i])
        {
        case SVM_INT:
        {
            printf("INT\n");
            break;
        }
        case SVM_DOUBLE:
        {
            printf("DOUBLE\n");
            break;
        }
        default:
        {
            exit(1);
        }
        }
    }
    printf("\n-- code --\n");

    uint8_t *p = svm->code;
    DInfo dinfo = {0};
    int param_len = 0;
    for (int i = 0; i < svm->code_size; ++i, p++)
    {
        OpcodeInfo *oinfo = &svm_opcode_info[*p];
        add_rowcode(&dinfo, *p);
        switch (*p)
        {
        case SVM_CAST_DOUBLE_TO_INT:
        case SVM_CAST_INT_TO_DOUBLE:
        case SVM_PUSH_DOUBLE:
        case SVM_POP_STATIC_DOUBLE:
        case SVM_PUSH_INT:
        case SVM_POP_STATIC_INT:
        case SVM_PUSH_STATIC_INT:
        case SVM_PUSH_STATIC_DOUBLE:
        case SVM_PUSH_FUNCTION:
        case SVM_PUSH_STACK_PT: //
        case SVM_POP_STACK_PT:  //
        case SVM_POP:
        case SVM_ADD_INT:
        case SVM_ADD_DOUBLE:
        case SVM_SUB_INT:
        case SVM_SUB_DOUBLE:
        case SVM_MUL_INT:
        case SVM_MUL_DOUBLE:
        case SVM_DIV_INT:
        case SVM_DIV_DOUBLE:
        case SVM_MOD_INT:
        case SVM_MOD_DOUBLE:
        case SVM_LT_INT:
        case SVM_LT_DOUBLE:
        case SVM_LE_INT:
        case SVM_LE_DOUBLE:
        case SVM_GT_INT:
        case SVM_GT_DOUBLE:
        case SVM_GE_INT:
        case SVM_GE_DOUBLE:
        case SVM_EQ_INT:
        case SVM_EQ_DOUBLE:
        case SVM_NE_INT:
        case SVM_NE_DOUBLE:
        case SVM_LOGICAL_AND:
        case SVM_LOGICAL_OR:
        case SVM_LOGICAL_NOT:
        case SVM_MINUS_INT:
        case SVM_MINUS_DOUBLE:
        case SVM_INCREMENT:
        case SVM_DECREMENT:
        case SVM_INVOKE:
        {
            //                printf("%s\n", oinfo->opname);
            add_opname(&dinfo, oinfo->opname);
            break;
        }
        default:
        {
            fprintf(stderr, "unknown opcode [%02x] in disasm\n", *p);
            exit(1);
        }
        }
        param_len = strlen(oinfo->parameter);
        for (int j = 0; j < param_len; ++j)
        {
            switch (oinfo->parameter[j])
            {
            case 'i':
            {
                uint8_t uv = *(++p);
                uint16_t op = (uint16_t)(uv << 8 | *(++p));
                add_uint16(&dinfo, op);
                add_rowcode(&dinfo, uv);
                add_rowcode(&dinfo, *p);
                i += 2;
                break;
            }
            default:
            {
                fprintf(stderr, "unknown opcode [%c] in disasm\n",
                        oinfo->parameter[j]);
                exit(1);
            }
            }
        }
        if (param_len == 0)
            add_padding(&dinfo);

        dump(&dinfo);
    }
}

static void parse(uint8_t *buf, SVM_VirtualMachine *svm)
{
    uint8_t *pos = buf;
    parse_header(&pos);
    svm->constant_pool_count = read_int(&pos);
    //    printf("constant_pool_count = %d\n", svm->constant_pool_count);
    svm->constant_pool = (SVM_Constant *)MEM_malloc(sizeof(SVM_Constant) *
                                                    svm->constant_pool_count);

    uint8_t type;
    for (int i = 0; i < svm->constant_pool_count; ++i)
    {
        switch (type = read_byte(&pos))
        {
        case SVM_INT:
        {
            int v = read_int(&pos);
            //                printf("constant[%d] = %d\n", i, v);
            svm->constant_pool[i].type = SVM_INT;
            svm->constant_pool[i].u.c_int = v;
            break;
        }
        case SVM_DOUBLE:
        {
            double dv = read_double(&pos);
            //                printf("constant[%d] = %f\n", i, dv);
            svm->constant_pool[i].type = SVM_DOUBLE;
            svm->constant_pool[i].u.c_double = dv;
            break;
        }
        default:
        {
            fprintf(stderr, "undefined constant type\n in parse");
            exit(1);
        }
        }
    }

    svm->global_variable_count = read_int(&pos);
    svm->global_variables =
        (SVM_Value *)MEM_malloc(sizeof(SVM_Value) * svm->global_variable_count);
    svm->global_variable_types =
        (uint8_t *)MEM_malloc(sizeof(uint8_t) * svm->global_variable_count);
    //    printf("global_variable_count = %d\n", svm->global_variable_count);
    for (int i = 0; i < svm->global_variable_count; ++i)
    {
        //        uint8_t type = read_byte(&pos);
        svm->global_variable_types[i] = read_byte(&pos);
        switch (svm->global_variable_types[i])
        {
        case SVM_INT:
        {
            //                printf("INT\n");
            break;
        }
        case SVM_DOUBLE:
        {
            //                printf("DOUBLE\n");
            break;
        }
        defulat:
        {
            break;
        }
        }
    }

    svm->code_size = read_int(&pos);
    svm->code = (uint8_t *)MEM_malloc(svm->code_size);
    memcpy(svm->code, pos, svm->code_size);
    pos += svm->code_size;
    svm->stack_size = read_int(&pos);
}

static SVM_VirtualMachine *svm_create()
{
    SVM_VirtualMachine *svm =
        (SVM_VirtualMachine *)MEM_malloc(sizeof(SVM_VirtualMachine));
    svm->constant_pool = NULL;
    svm->global_variables = NULL;
    svm->global_variable_types = NULL;
    svm->code = NULL;
    svm->constant_pool_count = 0;
    svm->global_variable_count = 0;
    svm->code_size = 0;
    svm->function_count = 0;
    svm->functions = NULL;
    svm->stack = NULL;
    svm->stack_size = 0;
    svm->pt_stack_size = 100; //
    svm->stack_value_type = NULL;
    svm->pt_stack_count = 0;
    svm->pt_stack = NULL;         //
    svm->pt_stack_address = NULL; //
    svm->pc = 0;
    svm->sp = 0;
    return svm;
}

static void svm_delete(SVM_VirtualMachine *svm)
{
    if (!svm)
        return;
    if (svm->code)
    {
        MEM_free(svm->code);
    }
    if (svm->constant_pool)
    {
        MEM_free(svm->constant_pool);
    }
    if (svm->global_variables)
    {
        MEM_free(svm->global_variables);
    }
    if (svm->global_variable_types)
    {
        MEM_free(svm->global_variable_types);
    }
    if (svm->functions)
    {
        MEM_free(svm->functions);
    }
    if (svm->stack)
    {
        MEM_free(svm->stack);
    }
    if (svm->stack_value_type)
    {
        MEM_free(svm->stack_value_type);
    }

    MEM_free(svm);
}

void svm_add_native_function(SVM_VirtualMachine *svm,
                             SVM_NativeFunction native_f, char *name,
                             int arg_count)
{
    svm->functions = (SVM_Function *)MEM_realloc(
        svm->functions, sizeof(SVM_Function) * (svm->function_count + 1));
    svm->functions[svm->function_count].f_type = NATIVE_FUNCTION;
    svm->functions[svm->function_count].name = name;
    svm->functions[svm->function_count].arg_count = arg_count;
    svm->functions[svm->function_count].u.n_func = native_f;
    svm->function_count++;
}

static uint8_t fetch(SVM_VirtualMachine *svm) { return svm->code[svm->pc++]; }

static uint16_t fetch2(SVM_VirtualMachine *svm)
{
    uint8_t v1 = fetch(svm);
    return (v1 << 8) | fetch(svm);
}

static SVM_Constant *read_static(SVM_VirtualMachine *svm, uint16_t idx)
{
    return &svm->constant_pool[idx];
}

static int read_static_int(SVM_VirtualMachine *svm, uint16_t idx)
{
    return read_static(svm, idx)->u.c_int;
}

static double read_static_double(SVM_VirtualMachine *svm, uint16_t idx)
{
    return read_static(svm, idx)->u.c_double;
}

static void push_i(SVM_VirtualMachine *svm, int iv)
{
    svm->stack[svm->sp].ival = iv;
    svm->stack_value_type[svm->sp] = SVM_INT;
    svm->sp++;
}

static void push_d(SVM_VirtualMachine *svm, double dv)
{
    svm->stack[svm->sp].dval = dv;
    svm->stack_value_type[svm->sp] = SVM_DOUBLE;
    svm->sp++;
}

static void push_pt(SVM_VirtualMachine *svm)
{
    svm->pt_stack[svm->pt_stack_count] = svm->sp;
    svm->pt_stack_count++;
}

static int pop_i(SVM_VirtualMachine *svm)
{
    --svm->sp;
    return svm->stack[svm->sp].ival;
}

static double pop_d(SVM_VirtualMachine *svm)
{
    --svm->sp;
    return svm->stack[svm->sp].dval;
}

static void write_i(SVM_Value *head, uint32_t offset, uint32_t idx, int iv)
{
    head[offset + idx].ival = iv;
}
static int read_i(SVM_Value *head, uint32_t offset, uint32_t idx)
{
    return head[offset + idx].ival;
}

static void write_d(SVM_Value *head, uint32_t offset, uint32_t idx, double dv)
{
    head[offset + idx].dval = dv;
}
static double read_d(SVM_Value *head, uint32_t offset, uint32_t idx)
{
    return head[offset + idx].dval;
}

static void write_global_i(SVM_VirtualMachine *svm, uint32_t idx, int iv)
{
    write_i(svm->global_variables, 0, idx, iv);
}
static int read_global_i(SVM_VirtualMachine *svm, uint32_t idx)
{
    return read_i(svm->global_variables, 0, idx);
}

static void write_global_d(SVM_VirtualMachine *svm, uint32_t idx, double dv)
{
    write_d(svm->global_variables, 0, idx, dv);
}
// static void write_stack_pt(SVM_VirtualMachine *svm,uint32_t idx)
// {

// }
static double read_global_d(SVM_VirtualMachine *svm, uint32_t idx)
{
    return read_d(svm->global_variables, 0, idx);
}

static void init_svm(SVM_VirtualMachine *svm)
{
    svm->stack = (SVM_Value *)MEM_malloc(sizeof(SVM_Value) * svm->stack_size);
    svm->stack_value_type =
        (uint8_t *)MEM_malloc(sizeof(uint8_t) * svm->stack_size);
    svm->pc = 0;
    svm->sp = 0;

    for (int i = 0; i < svm->global_variable_count; ++i)
    {
        switch (svm->global_variable_types[i])
        {
        case SVM_INT:
        {
            svm->global_variables[i].ival = 0;
            break;
        }
        case SVM_DOUBLE:
        {
            svm->global_variables[i].dval = 0.0;
            break;
        }
        default:
        {
            fprintf(stderr, "no such svm type\n");
            exit(1);
        }
        }
    }
}

static void show_status(SVM_VirtualMachine *svm)
{
    printf("\n< show SVM status >\n");
    printf("-- global variable ---\n");
    for (int i = 0; i < svm->global_variable_count; ++i)
    {
        switch (svm->global_variable_types[i])
        {
        case SVM_INT:
        {
            printf("[%d:svm_int] = %d\n", i, svm->global_variables[i].ival);
            break;
        }
        case SVM_DOUBLE:
        {
            printf("[%d:svm_dbl] = %f\n", i, svm->global_variables[i].dval);
            break;
        }
        default:
        {
            fprintf(stderr, "no such svm type\n");
            exit(1);
        }
        }
    }
    printf("\n--- stack ---\n");
    for (int i = (svm->sp - 1); i >= 0; --i)
    {
        switch (svm->stack_value_type[i])
        {
        case SVM_INT:
        {
            printf("[%d:svm_int] = %d\n", i, svm->stack[i].ival);
            break;
        }
        case SVM_DOUBLE:
        {
            printf("[%d:svm_dbl] = %f\n", i, svm->stack[i].dval);
            break;
        }
        default:
        {
            fprintf(stderr, "no such svm type\n");
            exit(1);
        }
        }
    }
}

static void svm_run(SVM_VirtualMachine *svm)
{
    bool running = true;

    uint8_t op = 0;
    while (running)
    {
        switch (op = fetch(svm))
        {
        case SVM_PUSH_INT:
        { // push from constant pool
            uint16_t s_idx = fetch2(svm);
            int v = read_static_int(svm, s_idx);
            push_i(svm, v);
            break;
        }
        case SVM_PUSH_DOUBLE:
        {
            uint16_t s_idx = fetch2(svm);
            double dv = read_static_double(svm, s_idx);
            push_d(svm, dv);
            break;
        }
        case SVM_POP_STATIC_INT:
        { // save i_val to global variable
            uint16_t s_idx = fetch2(svm);
            int iv = pop_i(svm);
            write_global_i(svm, s_idx, iv);
            //                show_status(svm);
            //                exit(1);
            break;
        }
        case SVM_POP_STATIC_DOUBLE:
        { // save d_val to global variable
            uint16_t s_idx = fetch2(svm);
            double dv = pop_d(svm);
            write_global_d(svm, s_idx, dv);
            //                exit(1);
            break;
        }
        case SVM_POP_STACK_PT: //
        {
            uint16_t s_idx = fetch2(svm);
        }
        case SVM_PUSH_STACK_PT: //
        {
            uint16_t s_idx = fetch2(svm);
            push_pt(svm);
            break;
        }
        case SVM_PUSH_STATIC_INT:
        {
            uint16_t s_idx = fetch2(svm);
            int iv = read_global_i(svm, s_idx);
            //                printf("iv = %d\n", iv);
            push_i(svm, iv);
            break;
        }
        case SVM_PUSH_STATIC_DOUBLE:
        {
            uint16_t s_idx = fetch2(svm);
            double dv = read_global_d(svm, s_idx);
            push_d(svm, dv);
            break;
        }
        case SVM_ADD_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 + iv1));
            break;
        }
        case SVM_ADD_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_d(svm, (dv2 + dv1));
            break;
        }
        case SVM_SUB_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 - iv1));
            break;
        }
        case SVM_SUB_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_d(svm, (dv2 - dv1));
            break;
        }
        case SVM_MUL_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 * iv1));
            break;
        }
        case SVM_MUL_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_d(svm, (dv2 * dv1));
            break;
        }
        case SVM_DIV_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 / iv1));
            break;
        }
        case SVM_DIV_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_d(svm, (dv2 / dv1));
            break;
        }
        case SVM_MOD_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 % iv1));
            break;
        }
        case SVM_MOD_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_d(svm, fmod(dv2, dv1));
            break;
        }
        case SVM_LT_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 < iv1) ? 1 : 0);
            break;
        }
        case SVM_LT_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_i(svm, (dv2 < dv1) ? 1 : 0);
            break;
        }
        case SVM_LE_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 <= iv1) ? 1 : 0);
            break;
        }
        case SVM_LE_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_i(svm, (dv2 <= dv1) ? 1 : 0);
            break;
        }
        case SVM_GT_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 > iv1) ? 1 : 0);
            break;
            break;
        }
        case SVM_GT_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_i(svm, (dv2 > dv1) ? 1 : 0);
            break;
        }
        case SVM_GE_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 >= iv1) ? 1 : 0);
            break;
        }
        case SVM_GE_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_i(svm, (dv2 >= dv1) ? 1 : 0);
            break;
        }
        case SVM_EQ_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 == iv1) ? 1 : 0);
            break;
        }
        case SVM_EQ_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_i(svm, (dv2 == dv1) ? 1 : 0);
            break;
        }
        case SVM_NE_INT:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv2 != iv1) ? 1 : 0);
            break;
        }
        case SVM_NE_DOUBLE:
        {
            double dv1 = pop_d(svm);
            double dv2 = pop_d(svm);
            push_i(svm, (dv2 != dv1) ? 1 : 0);
            break;
        }
        case SVM_CAST_DOUBLE_TO_INT:
        {
            double dv = pop_d(svm);
            push_i(svm, (int)dv);
            break;
        }
        case SVM_INCREMENT:
        {
            int iv = pop_i(svm);
            push_i(svm, ++iv);
            break;
        }
        case SVM_DECREMENT:
        {
            int iv = pop_i(svm);
            push_i(svm, --iv);
            break;
        }
        case SVM_LOGICAL_AND:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv1 == 1 && iv2 == 1) ? 1 : 0);
            break;
        }
        case SVM_LOGICAL_OR:
        {
            int iv1 = pop_i(svm);
            int iv2 = pop_i(svm);
            push_i(svm, (iv1 == 1 || iv2 == 1) ? 1 : 0);
            break;
        }
        case SVM_LOGICAL_NOT:
        {
            int iv = pop_i(svm);
            push_i(svm, (iv == 1) ? 0 : 1);
            break;
        }
        case SVM_MINUS_INT:
        {
            int iv = pop_i(svm);
            push_i(svm, -iv);
            break;
        }
        case SVM_MINUS_DOUBLE:
        {
            double dv = pop_d(svm);
            push_d(svm, -dv);
            break;
        }
        case SVM_CAST_INT_TO_DOUBLE:
        {
            int i = pop_i(svm);
            push_d(svm, (double)i);
            break;
        }
        case SVM_PUSH_FUNCTION:
        {
            uint16_t idx = fetch2(svm);
            push_i(svm, idx);
            break;
        }
        case SVM_INVOKE:
        {
            uint16_t f_idx = pop_i(svm);
            switch (svm->functions[f_idx].f_type)
            {
            case NATIVE_FUNCTION:
            {
                SVM_Value val = svm->functions[f_idx].u.n_func(
                    svm,
                    &svm->stack[svm->sp -
                                svm->functions[f_idx].arg_count],
                    svm->functions[f_idx].arg_count);
                svm->sp -= svm->functions[f_idx].arg_count;
                svm->stack[svm->sp++] = val;
                break;
            }
            default:
            {
                fprintf(stderr, "no such function type in invoke\n");
                exit(1);
            }
            }
            break;
        }
        case SVM_POP:
        {
            pop_i(svm);
            break;
        }
        default:
        {
            fprintf(stderr, "unknown opcode: %02x in svm_run\n", op);
            show_status(svm);
            exit(1);
        }
        }

        running = svm->pc < svm->code_size;
    }
    show_status(svm);
}

int main(int argc, char *argv[])
{
    // for test
    bool disasm_mode = false;
    int file_idx = 1;
    if (argc < 2)
    {
        fprintf(stderr, "Usage ./svm [optino] file\n");
        exit(1);
    }

    if (argc == 3)
    {
        if (!strcmp("-d", argv[1]))
        {
            printf("disasm\n");
            file_idx++;
            disasm_mode = true;
        }
        else
        {
            fprintf(stderr, "No such option)\n");
        }
    }

    SVM_VirtualMachine *svm = svm_create();
    struct stat st;
    stat(argv[file_idx], &st);
    uint8_t *buf = (uint8_t *)malloc(st.st_size);
    int fp = open(argv[file_idx], O_RDONLY);
    read(fp, buf, st.st_size);
    parse(buf, svm);
    close(fp);
    free(buf);

    if (disasm_mode)
    {
        disasm(svm);
    }
    else
    {
        add_native_functions(svm);
        init_svm(svm);
        svm_run(svm);
    }

    svm_delete(svm);
    MEM_dump_memory();

    return 0;
}