#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../memory/MEM.h"
#include "../svm/svm.h"
#include "csua.h"
#include "visitor.h"

static void copy_declaration(CS_Compiler* compiler, CS_Executable* exec) {
    DeclarationList* decl_list = compiler->decl_list;
    int size;
    for (size = 0; decl_list; decl_list = decl_list->next, ++size)
        ;
    CS_Variable* variables =
        (CS_Variable*)MEM_malloc(sizeof(CS_Variable) * size);
    decl_list = compiler->decl_list;
    for (int i = 0; i < size; decl_list = decl_list->next, ++i) {
        variables[i].name = MEM_strdup(decl_list->decl->name);
        TypeSpecifier* type = MEM_malloc(sizeof(TypeSpecifier));
        type->basic_type = decl_list->decl->type->basic_type;
        variables[i].type = type;
    }
    exec->global_variable = variables;
    exec->global_variable_count = size;
}

static CS_Executable* code_generate(CS_Compiler* compiler) {
    CS_Executable* exec = (CS_Executable*)MEM_malloc(sizeof(CS_Executable));
    memset(exec, 0x0, sizeof(CS_Executable));
    exec->code = NULL;
    exec->code_size = 0;
    exec->constant_pool = NULL;
    exec->constant_pool_count = 0;
    exec->global_variable = NULL;
    exec->global_variable_count = 0;

    copy_declaration(compiler, exec);  // copy variables
    CodegenVisitor* cgen_visitor = create_codegen_visitor(compiler, exec);

    StatementList* stmt_list = compiler->stmt_list;
    while (stmt_list) {
        traverse_stmt(stmt_list->stmt, (Visitor*)cgen_visitor);
        stmt_list = stmt_list->next;
    }

    exec->code_size = cgen_visitor->pos;
    exec->code = (uint8_t*)MEM_malloc(exec->code_size);
    memcpy(exec->code, cgen_visitor->code, exec->code_size);

    if (cgen_visitor->code) {
        MEM_free(cgen_visitor->code);
    }
    delete_visitor((Visitor*)cgen_visitor);

    return exec;
}

static void delete_executable(CS_Executable* exec) {
    for (int i = 0; i < exec->global_variable_count; ++i) {
        MEM_free(exec->global_variable[i].name);
        MEM_free(exec->global_variable[i].type);
    }
    MEM_free(exec->global_variable);

    if (exec->constant_pool != NULL) {
        MEM_free(exec->constant_pool);
    }

    if (exec->code != NULL) {
        MEM_free(exec->code);
    }

    MEM_free(exec);
}

typedef struct {
    char buf[128];
    int index;
} DInfo;

static void dump(DInfo* info) {
    fprintf(stderr, "%s\n", info->buf);
    info->index = 0;
}

static void add_string(DInfo* info, const char* str) {
    int len = strlen(str);
    strncpy(&info->buf[info->index], str, len);
    info->buf[info->index += len] = 0;
}

static void add_uint16(DInfo* info, const uint16_t iv) {
    char buf[6];
    buf[0] = 0x20;
    sprintf(&buf[1], "%04x", iv);
    buf[5] = 0;
    add_string(info, buf);
}

static void write_char(char c, FILE* fp) { fwrite(&c, 1, 1, fp); }

static void write_reverse(const void* pv, size_t size, FILE* fp) {
    char* p = (char*)pv;
    for (int i = size - 1; i >= 0; --i) {
        write_char(p[i], fp);
    }
}

static void write_int(const uint32_t v, FILE* fp) {
    write_reverse(&v, sizeof(uint32_t), fp);
}

static void write_double(const double dv, FILE* fp) {
    write_reverse(&dv, sizeof(double), fp);
}

static void write_bytes(uint8_t* p, int len, FILE* fp) {
    fwrite(p, 1, len, fp);
}

static int count_stack_size(uint8_t* code, size_t len) {
    int st_size = 0;
    for (int i = 0; i < len; ++i) {
        OpcodeInfo* oinfo = &svm_opcode_info[code[i]];
        if (oinfo->s_size > 0) {
            st_size += oinfo->s_size;
        }
        for (int j = 0; j < strlen(oinfo->parameter); ++j) {
            switch (oinfo->parameter[j]) {
                case 'i': {
                    i += 2;
                    break;
                }
                default: {
                    fprintf(stderr, "unknown parameter [%c]in disassemble\n",
                            oinfo->parameter[j]);
                    exit(1);
                }
            }
        }
    }
    return st_size;
}

static void serialize(CS_Executable* exec) {
    FILE* fp;

    if ((fp = fopen("../svm/a.csb", "wb")) == NULL) {
        fprintf(stderr, "Error\n");
        exit(1);
    }
    write_char('C', fp);
    write_char('A', fp);
    write_char('P', fp);
    write_char('H', fp);
    write_char('E', fp);
    write_char('S', fp);
    write_char('U', fp);
    write_char('A', fp);

    //    printf("global len = %d\n", exec->global_variable_count);
    write_int(exec->constant_pool_count, fp);
    for (int i = 0; i < exec->constant_pool_count; ++i) {
        switch (exec->constant_pool[i].type) {
            case CS_CONSTANT_INT: {
                write_char(SVM_INT, fp);
                write_int(exec->constant_pool[i].u.c_int, fp);
                break;
            }
            case CS_CONSTANT_DOUBLE: {
                //                printf("write double\n");
                //                printf("dval = %f\n",
                //                exec->constant_pool[i].u.c_double);
                write_char(SVM_DOUBLE, fp);
                write_double(exec->constant_pool[i].u.c_double, fp);
                break;
            }
            default: {
                fprintf(stderr, "undefined constant type\n in disasm");
                exit(1);
            }
        }
    }

    write_int(exec->global_variable_count, fp);
    for (int i = 0; i < exec->global_variable_count; ++i) {
        switch (exec->global_variable[i].type->basic_type) {
            case CS_BOOLEAN_TYPE:
            case CS_INT_TYPE: {
                write_char(SVM_INT, fp);
                break;
            }
            case CS_DOUBLE_TYPE: {
                write_char(SVM_DOUBLE, fp);
                break;
            }
            default: {
                fprintf(stderr, "No such type\n");
                exit(1);
            }
        }
    }

    write_int(exec->code_size, fp);
    write_bytes(exec->code, exec->code_size, fp);
    int stack_size = count_stack_size(exec->code, exec->code_size);
    //    printf("s_size = %d\n", stack_size);
    write_int(stack_size, fp);

    fclose(fp);
}

static void exec_disasm(CS_Executable* exec) {
    fprintf(stderr, "< Disassemble Start >\n");
    fprintf(stderr, "-- global variables --\n");
    for (int i = 0; i < exec->global_variable_count; ++i) {
        if (i % 10 == 0) fprintf(stderr, "\n");
        fprintf(stderr, "[%d]%s:%s ", i, exec->global_variable[i].name,
                get_type_name(exec->global_variable[i].type->basic_type));
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "-- constant pool --\n");
    fprintf(stderr, "pool count = %d\n", exec->constant_pool_count);
    for (int i = 0; i < exec->constant_pool_count; ++i) {
        fprintf(stderr, "[%d]:", i);
        switch (exec->constant_pool[i].type) {
            case CS_CONSTANT_INT: {
                fprintf(stderr, "%d\n", exec->constant_pool[i].u.c_int);
                break;
            }
            case CS_CONSTANT_DOUBLE: {
                fprintf(stderr, "%f\n", exec->constant_pool[i].u.c_double);
                break;
            }
            default: {
                fprintf(stderr, "undefined constant type\n in disasm");
                exit(1);
            }
        }
    }

    fprintf(stderr, "-- code --\n");
    for (int i = 0; i < exec->code_size; ++i) {
        if (i % 16 == 0) fprintf(stderr, "\n");
        fprintf(stderr, "%02x ", exec->code[i]);
    }
    fprintf(stderr, "\n\n");
    DInfo dinfo;
    dinfo.index = 0;
    //    add_string(&dinfo, "abc");
    //    add_string(&dinfo, "def");
    //    add_uint16(&dinfo, 10);
    //    dump(&dinfo);
    uint8_t* code = exec->code;

    for (int i = 0; i < exec->code_size; ++i) {
        OpcodeInfo* oinfo = &svm_opcode_info[code[i]];
        switch (code[i]) {
            case SVM_CAST_DOUBLE_TO_INT:
            case SVM_CAST_INT_TO_DOUBLE:
            case SVM_PUSH_DOUBLE:
            case SVM_POP_STATIC_DOUBLE:
            case SVM_PUSH_INT:
            case SVM_POP_STATIC_INT:
            case SVM_PUSH_STACK_PT:  //
            case SVM_POP_STACK_PT:   //
            case SVM_PUSH_STATIC_INT:
            case SVM_PUSH_STATIC_DOUBLE:
            case SVM_PUSH_FUNCTION:
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
            case SVM_INVOKE: {
                add_string(&dinfo, oinfo->opname);
                break;
            }
            default: {
                fprintf(stderr, "unknown opcode [%02x]in disassemble\n",
                        code[i]);
                exit(1);
            }
        }
        for (int j = 0; j < strlen(oinfo->parameter); ++j) {
            switch (oinfo->parameter[j]) {
                case 'i': {
                    uint8_t uv = code[++i];
                    uint16_t op = (uint16_t)(uv << 8 | code[++i]);
                    add_uint16(&dinfo, op);
                    break;
                }
                default: {
                    fprintf(stderr, "unknown parameter [%c]in disassemble\n",
                            oinfo->parameter[j]);
                    exit(1);
                }
            }
        }
        dump(&dinfo);
    }

    fprintf(stderr, "\n< Disassemble End >\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage ./meant dir/filename.cs\n");
        return 1;
    }
    FILE* fin = fopen(argv[1], "r");
    if (fin == NULL) {
        printf("Cannot fine file %s\n", argv[1]);
        return 1;
    }
    CS_Compiler* compiler = CS_create_compiler();
    CS_Boolean compile_result = CS_compile(compiler, fin);

    if (compile_result) {
        // Code Generate
        CS_Executable* exec = code_generate(compiler);
        exec_disasm(exec);
        serialize(exec);
        delete_executable(exec);

        fprintf(stderr, "\n--- Tree View ---\n");
        Visitor* visitor = create_treeview_visitor();
        StatementList* stmt_list = compiler->stmt_list;
        while (stmt_list) {
            traverse_stmt(stmt_list->stmt, visitor);
            stmt_list = stmt_list->next;
        }
        delete_visitor(visitor);
    }

    fclose(fin);
    CS_delete_compiler(compiler);
    MEM_dump_memory();
    return 0;
}