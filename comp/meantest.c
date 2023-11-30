#include <stdio.h>
#include <stdlib.h>

#include "../memory/MEM.h"
#include "csua.h"
#include "visitor.h"

int main(int argc, char* argv[]) {
    printf("arg len = %d\n", argc);

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
        printf("--------------\n");
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