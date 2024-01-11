
#include <stdio.h>
#include <string.h>

#include "csua.h"

static CS_Compiler* current_compiler = NULL;

void cs_set_current_compiler(CS_Compiler* compiler) {
    current_compiler = compiler;
}

CS_Compiler* cs_get_current_compiler() { return current_compiler; }

DeclarationList* cs_chain_declaration(DeclarationList* decl_list,
                                      Declaration* decl) {
    // DeclarationList* p;
    // DeclarationList* list = cs_create_declaration_list(decl);
    // if (decl_list == NULL) return list;
    // for (p = decl_list; p->next; p = p->next);
    // p->next = list;

    // 新しいノードを作成
    DeclarationList* new_tail = cs_create_declaration_list(decl);

    CS_Compiler* compiler = cs_get_current_compiler();
    if (decl_list == NULL) {
        // リストが空ならリストの末尾を新しいノードにする
        compiler->decl_list_tail = new_tail;
        return new_tail;
    }
    // decl_list_tailの次に新しいノードを追加
    compiler->decl_list_tail->next = new_tail;
    new_tail->prev = compiler->decl_list_tail;
    // decl_list_tailを更新
    compiler->decl_list_tail = new_tail;

    return decl_list;
}

StatementList* cs_chain_statement_list(StatementList* stmt_list,
                                       Statement* stmt) {
    StatementList* p = NULL;
    StatementList* nstmt_list = cs_create_statement_list(stmt);
    if (stmt_list == NULL) {
        return nstmt_list;
    }
    for (p = stmt_list; p->next; p = p->next)
        ;
    p->next = nstmt_list;

    return stmt_list;
}

FunctionDeclarationList* cs_chain_function_declaration_list(
    FunctionDeclarationList* func_list, FunctionDeclaration* func) {
    // FunctionDeclarationList* p = NULL;
    // FunctionDeclarationList* nfunc_list =
    //     cs_create_function_declaration_list(func);
    // if (func_list == NULL) {
    //     return nfunc_list;
    // }
    // for (p = func_list; p->next; p = p->next);
    // p->next = nfunc_list;

    // 新しいノードを作成
    FunctionDeclarationList* new_tail =
        cs_create_function_declaration_list(func);

    CS_Compiler* compiler = cs_get_current_compiler();
    if (func_list == NULL) {
        // リストが空ならリストの末尾を新しいノードにする
        compiler->func_list_tail = new_tail;
        return new_tail;
    }
    // func_list_tailの次に新しいノードを追加
    compiler->func_list_tail->next = new_tail;
    new_tail->prev = compiler->func_list_tail;
    // func_list_tailを更新
    compiler->func_list_tail = new_tail;

    return func_list;
}

ParameterList* cs_chain_parameter_list(ParameterList* list, CS_BasicType type,
                                       char* name) {
    ParameterList* p = NULL;
    ParameterList* current = cs_create_parameter(type, name);
    for (p = list; p->next; p = p->next)
        ;
    p->next = current;
    return list;
}

ArgumentList* cs_chain_argument_list(ArgumentList* list, Expression* expr) {
    ArgumentList* p;
    ArgumentList* current = cs_create_argument(expr);
    for (p = list; p->next; p = p->next)
        ;
    p->next = current;
    return list;
}

static Declaration* search_decls_from_list(DeclarationList* list,
                                           const char* name) {
    for (; list; list = list->next) {
        if (!strcmp(list->decl->name, name)) {
            return list->decl;
        }
    }
    return NULL;
}
// search from a block temporary
/// @brief 有効なスコープ内で変数を探索する
/// @return 変数が見つからない場合はNULLを返す
Declaration* cs_search_decl_in_block(const char* name,
                                     DeclarationList* decl_list_border,
                                     CheckpointList* cp_list_boarder) {
    CheckpointList* cp_list = cp_list_boarder;

    // スキップすべき最初のCheckpoint(BLOCK_OPE_END:'}')を見つけるまでCheckpointStackを逆順にたどる
    for (; cp_list != NULL && cp_list->checkpoint->type == BLOCK_OPE_BEGIN;
         cp_list = cp_list->prev)
        ;

    DeclarationList* list = decl_list_border;

    while (list != NULL) {
        if (cp_list != NULL && list == cp_list->checkpoint->decl_list_ptr &&
            cp_list->prev != NULL) {
            fprintf(stderr, "Skip block: [decl=%p->%p]\n",
                    cp_list->checkpoint->decl_list_ptr,
                    cp_list->prev->checkpoint->decl_list_ptr);
            // Checkpoint( BLOCK_OPE_END:'}' )に到達
            // 次のチェックポイント( BLOCK_OPE_BEGIN:'{' )までlistをスキップする
            list = cp_list->prev->checkpoint->decl_list_ptr;
            // Checkpointを更新
            cp_list = cp_list->prev->prev;
            continue;
        }

        if (strcmp(list->decl->name, name) == 0) {
            return list->decl;
        }

        list = list->prev;
    }
    fprintf(stderr, "Not Found variable\n");
    return NULL;
}

Declaration* cs_search_decl_global(const char* name) {
    CS_Compiler* compiler = cs_get_current_compiler();
    return search_decls_from_list(compiler->decl_list, name);
}

static FunctionDeclaration* search_function_from_list(
    FunctionDeclarationList* list, const char* name) {
    for (; list; list = list->next) {
        if (!strcmp(list->func->name, name)) {
            return list->func;
        }
    }
    return NULL;
}

FunctionDeclaration* cs_search_func_in_block(
    const char* name, FunctionDeclarationList* func_decl_list_border,
    CheckpointList* cp_list_boarder) {
    CheckpointList* cp_list = cp_list_boarder;

    // スキップすべき最初のCheckpoint(BLOCK_OPE_END:'}')を見つけるまでCheckpointStackを逆順にたどる
    for (; cp_list != NULL && cp_list->checkpoint->type == BLOCK_OPE_BEGIN;
         cp_list = cp_list->prev)
        ;

    FunctionDeclarationList* list = func_decl_list_border;

    while (list != NULL) {
        if (cp_list != NULL && list == cp_list->checkpoint->fun_list_ptr) {
            fprintf(stderr, "Skip block: [decl=%p->%p]\n",
                    cp_list->checkpoint->fun_list_ptr,
                    cp_list->prev->checkpoint->fun_list_ptr);
            // Checkpoint( BLOCK_OPE_END:'}' )に到達
            // 次のチェックポイント( BLOCK_OPE_BEGIN:'{' )までlistをスキップする
            list = cp_list->prev->checkpoint->fun_list_ptr;
            // Checkpointを更新
            cp_list = cp_list->prev->prev;
            continue;
        }

        if (strcmp(list->func->name, name) == 0) {
            return list->func;
        }

        list = list->prev;
    }
    return NULL;
}

FunctionDeclaration* cs_search_function(const char* name) {
    CS_Compiler* compiler = cs_get_current_compiler();
    return search_function_from_list(compiler->func_list, name);
}

static Checkpoint* create_checkpoint(BlockOperationType type) {
    Checkpoint* cp = (Checkpoint*)cs_malloc(sizeof(Checkpoint));
    cp->type = type;

    CS_Compiler* compiler = cs_get_current_compiler();
    cp->decl_list_ptr = compiler->decl_list_tail;
    cp->fun_list_ptr = compiler->func_list_tail;

    fprintf(stderr,
            "Create Checkpoint: [type=%d, decl_list=%p, func_list=%p]\n", type,
            cp->decl_list_ptr, cp->fun_list_ptr);

    return cp;
}
static CheckpointList* create_checkpoint_list(Checkpoint* cp) {
    CheckpointList* cp_list =
        (CheckpointList*)cs_malloc(sizeof(CheckpointList));
    cp_list->next = NULL;
    cp_list->prev = NULL;
    cp_list->checkpoint = cp;
    return cp_list;
}
/// @brief Checkpointを作成してスタックに積む
/// @param type 新しいCheckpointのBlockOperationType
void cs_record_checkpoint(BlockOperationType type) {
    Checkpoint* cp = create_checkpoint(type);
    CheckpointList* new_list = create_checkpoint_list(cp);

    CS_Compiler* compiler = cs_get_current_compiler();
    if (compiler->cp_list == NULL) {
        compiler->cp_list = new_list;
        compiler->cp_list_tail = new_list;
        return;
    }

    compiler->cp_list_tail->next = new_list;
    new_list->prev = compiler->cp_list_tail;

    compiler->cp_list_tail = new_list;
}
