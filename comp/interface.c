
#include <stdio.h>
#include <stdlib.h>

#include "csua.h"
#include "visitor.h"

CS_Compiler* CS_create_compiler() {
    MEM_Storage storage;
    CS_Compiler* compiler;
    storage = MEM_open_storage(0);
    compiler = (CS_Compiler*)MEM_storage_malloc(storage, sizeof(CS_Compiler));
    compiler->storage = storage;
    compiler->expr_list = NULL;
    compiler->stmt_list = NULL;
    compiler->decl_list = NULL;
    compiler->func_list = NULL;
    compiler->current_line = 1;

    compiler->decl_list_tail = NULL;
    compiler->func_list_tail = NULL;
    compiler->cp_list = NULL;
    compiler->cp_list_tail = NULL;

    cs_set_current_compiler(compiler);

    return compiler;
}

void CS_delete_compiler(CS_Compiler* compiler) {
    MEM_Storage storage = compiler->storage;
    MEM_dispose(storage);
}

static CS_Boolean do_mean_check(CS_Compiler* compiler) {
    MeanVisitor* mean_visitor = create_mean_visitor();

    StatementList* stmt_list = compiler->stmt_list;
    while (stmt_list) {
        traverse_stmt(stmt_list->stmt, (Visitor*)mean_visitor);
        stmt_list = stmt_list->next;
    }

    DeclarationList* dp = NULL;
    dp = compiler->decl_list;
    for (int i = 0; dp; dp = dp->next, ++i) {
        dp->decl->index = i;
    }

    FunctionDeclarationList* func_list = compiler->func_list;
    for (int i = 0; func_list; func_list = func_list->next, ++i) {
        func_list->func->index = i;
    }

    /*
        func_list = compiler->func_list;
        for (; func_list; func_list = func_list->next) {
            printf("func name[%d] = %s\n", func_list->func->index,
       func_list->func->name);
        }

        dp = compiler->decl_list;
        for (int i = 0; dp; dp = dp->next, ++i) {
            printf("index = %d\n", dp->decl->index);
        }
    */
    if (mean_visitor->check_log != NULL) {
        show_mean_error(mean_visitor);
        delete_visitor((Visitor*)mean_visitor);
        return CS_FALSE;
    } else {
        delete_visitor((Visitor*)mean_visitor);
        return CS_TRUE;
    }
}

CS_Boolean CS_compile(CS_Compiler* compiler, FILE* fin) {
    extern int yyparse(void);
    extern FILE* yyin;
    yyin = fin;
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }
    if (yyparse()) {
        fprintf(stderr, "Parse Error");
        exit(1);
    }

    return do_mean_check((compiler));
}
