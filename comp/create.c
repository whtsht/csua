#include <stdio.h>
#include <string.h>

#include "../memory/MEM.h"
#include "csua.h"

static MEM_Storage storage = NULL;
static CS_Compiler *compiler = NULL;

static int *linenum = NULL;

/* for debug */
extern int current_line;

static void init_storage() {
    if (storage == NULL) {
#ifdef STORAGE
        storage = MEM_open_storage(0);
        printf("init_storage\n");
        linenum = &current_line;

#else
        storage = cs_get_current_compiler()->storage;
        linenum = &cs_get_current_compiler()->current_line;

#endif
    }
}

void *cs_malloc(size_t size) {
    init_storage();
    return MEM_storage_malloc(storage, size);
}

static Expression *cs_create_expression(ExpressionKind ekind) {
    Expression *expr = (Expression *)cs_malloc(sizeof(Expression));
    expr->kind = ekind;
    expr->type = NULL;
    expr->line_number = *linenum;
    return expr;
}

Expression *cs_create_int_expression(int v) {
    Expression *expr = cs_create_expression(INT_EXPRESSION);
    expr->u.int_value = v;
    return expr;
}

void delete_storage() {
#ifdef STORAGE
    if (storage != NULL) {
        MEM_dispose(storage);
    }
#endif
}

ExpressionList *cs_chain_expression_list(ExpressionList *list,
                                         Expression *expr) {
    ExpressionList *p = list;
    ExpressionList *nlist =
        (ExpressionList *)MEM_storage_malloc(storage, sizeof(ExpressionList));
    nlist->next = NULL;
    nlist->expression = expr;
    if (p != NULL) {
        while (p->next) p = p->next;
        p->next = nlist;
        return list;
    }
    return nlist;
}

Expression *cs_create_double_expression(double v) {
    Expression *expr = cs_create_expression(DOUBLE_EXPRESSION);
    expr->u.double_value = v;
    return expr;
}

Expression *cs_create_boolean_expression(CS_Boolean v) {
    Expression *expr = cs_create_expression(BOOLEAN_EXPRESSION);
    expr->u.boolean_value = v;
    return expr;
}

Expression *cs_create_identifier_expression(char *identifier) {
    Expression *expr = cs_create_expression(IDENTIFIER_EXPRESSION);
    expr->u.identifier.name = identifier;
    return expr;
}

Expression *cs_create_inc_dec_expression(Expression *id_expr,
                                         ExpressionKind inc_dec) {
    Expression *expr = cs_create_expression(inc_dec);
    expr->u.inc_dec = id_expr;
    return expr;
}

// args is argument not yet
Expression *cs_create_function_call_expression(Expression *function,
                                               ArgumentList *args) {
    Expression *expr = cs_create_expression(FUNCTION_CALL_EXPRESSION);
    expr->u.function_call_expression.function = function;
    expr->u.function_call_expression.argument = args;
    return expr;
}

Expression *cs_create_minus_expression(Expression *operand) {
    Expression *expr = cs_create_expression(MINUS_EXPRESSION);
    expr->u.minus_expression = operand;
    return expr;
}

Expression *cs_create_logical_not_expression(Expression *operand) {
    Expression *expr = cs_create_expression(LOGICAL_NOT_EXPRESSION);
    expr->u.logical_not_expression = operand;
    return expr;
}

Expression *cs_create_binary_expression(ExpressionKind kind, Expression *left,
                                        Expression *right) {
    Expression *expr = cs_create_expression(kind);
    expr->u.binary_expression.left = left;
    expr->u.binary_expression.right = right;
    return expr;
}

Expression *cs_create_assignment_expression(Expression *left,
                                            AssignmentOperator aope,
                                            Expression *operand) {
    Expression *expr = cs_create_expression(ASSIGN_EXPRESSION);
    expr->u.assignment_expression.aope = aope;
    expr->u.assignment_expression.left = left;
    expr->u.assignment_expression.right = operand;
    return expr;
}

Expression *cs_create_cast_expression(CS_CastType ctype, Expression *operand) {
    Expression *expr = cs_create_expression(CAST_EXPRESSION);
    expr->u.cast_expression.ctype = ctype;
    expr->u.cast_expression.expr = operand;
    return expr;
}

char *cs_create_identifier(const char *str) {
    char *new_char;
    new_char = (char *)cs_malloc(strlen(str) + 1);
    strcpy(new_char, str);
    return new_char;
}

/* For Statement */
static Statement *cs_create_statement(StatementType type) {
    // Expression* expr = (Expression*)cs_malloc(sizeof(Expression));
    Statement *stmt = (Statement *)cs_malloc(sizeof(Statement));
    stmt->type = type;
    stmt->line_number = *linenum;
    return stmt;
}

Statement *cs_create_expression_statement(Expression *expr) {
    Statement *stmt = cs_create_statement(EXPRESSION_STATEMENT);
    stmt->u.expression_s = expr;
    return stmt;
}

TypeSpecifier *cs_create_type_specifier(CS_BasicType type) {
    TypeSpecifier *ts = (TypeSpecifier *)cs_malloc(sizeof(TypeSpecifier));
    ts->basic_type = type;

    return ts;
}

ParameterList *cs_create_parameter(CS_BasicType type, char *name) {
    ParameterList *param = (ParameterList *)cs_malloc(sizeof(ParameterList));
    param->type = cs_create_type_specifier(type);
    param->name = name;
    param->line_number = *linenum;
    param->next = NULL;
    return param;
}

static Declaration *cs_create_declaration(CS_BasicType type, char *name,
                                          Expression *initializer) {
    Declaration *decl = (Declaration *)cs_malloc(sizeof(Declaration));
    decl->type = cs_create_type_specifier(type);
    decl->name = name;
    decl->initializer = initializer;
    decl->index = -1;
    return decl;
}

Statement *cs_create_declaration_statement(CS_BasicType type, char *name,
                                           Expression *initializer) {
    Statement *stmt = cs_create_statement(DECLARATION_STATEMENT);
    stmt->u.declaration_s = cs_create_declaration(type, name, initializer);
    return stmt;
}

StatementList *cs_create_statement_list(Statement *stmt) {
    StatementList *stmt_list =
        (StatementList *)cs_malloc(sizeof(StatementList));
    stmt_list->stmt = stmt;
    stmt_list->next = NULL;
    return stmt_list;
}

DeclarationList *cs_create_declaration_list(Declaration *decl) {
    DeclarationList *list = cs_malloc(sizeof(DeclarationList));
    list->next = NULL;
    list->prev = NULL;
    list->decl = decl;
    return list;
}

FunctionDeclaration *cs_create_function_declaration(CS_BasicType type,
                                                    char *name,
                                                    ParameterList *param) {
    FunctionDeclaration *decl =
        (FunctionDeclaration *)cs_malloc(sizeof(FunctionDeclaration));
    decl->type = cs_create_type_specifier(type);
    decl->name = name;
    decl->param = param;
    decl->index = -1;
    return decl;
}

FunctionDeclarationList *cs_create_function_declaration_list(
    FunctionDeclaration *func) {
    FunctionDeclarationList *list = cs_malloc(sizeof(FunctionDeclarationList));
    list->next = NULL;
    list->prev = NULL;
    list->func = func;
    return list;
}

ArgumentList *cs_create_argument(Expression *expr) {
    ArgumentList *argument = cs_malloc(sizeof(ArgumentList));
    argument->expr = expr;
    argument->next = NULL;
    return argument;
}

static BlockOperation *cs_create_block_operation(BlockOperationType type) {
    BlockOperation *block_ope = cs_malloc(sizeof(BlockOperation));
    block_ope->type = type;
    return block_ope;
}
Statement *cs_create_block_begin_statement() {
    Statement *stmt = cs_create_statement(BLOCKOPERATION_STATEMENT);
    stmt->u.blockop_s = cs_create_block_operation(BLOCK_OPE_BEGIN);
    return stmt;
}
Statement *cs_create_block_end_statement() {
    Statement *stmt = cs_create_statement(BLOCKOPERATION_STATEMENT);
    stmt->u.blockop_s = cs_create_block_operation(BLOCK_OPE_END);
    return stmt;
}

Statement *cs_create_if_begin_statement(Expression *expr) {
    Statement *stmt = cs_create_statement(IF_STATEMENT);

    stmt->u.ifop_s = cs_malloc(sizeof(IfOperation));
    stmt->u.ifop_s->expression_s = expr;
    stmt->u.ifop_s->op_kind = IF_OP_ENTER;

    return stmt;
}

Statement *cs_create_if_end_statement() {
    Statement *stmt = cs_create_statement(IF_STATEMENT);
    stmt->u.ifop_s = cs_malloc(sizeof(IfOperation));
    stmt->u.ifop_s->op_kind = IF_OP_LEAVE;
    return stmt;
}
