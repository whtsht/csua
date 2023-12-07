
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../memory/MEM.h"
#include "visitor.h"

#define cs_is_type(type, cs_type) ((type)->basic_type == (cs_type))

#define cs_is_int(type) (cs_is_type(type, CS_INT_TYPE))
#define cs_is_double(type) (cs_is_type(type, CS_DOUBLE_TYPE))
#define cs_is_boolean(type) (cs_is_type(type, CS_BOOLEAN_TYPE))

#define cs_same_type(type1, type2) ((type1)->basic_type == (type2)->basic_type)

char* get_type_name(CS_BasicType type) {
    switch (type) {
        case CS_BOOLEAN_TYPE: {
            return "boolean";
        }
        case CS_INT_TYPE: {
            return "int";
        }
        case CS_DOUBLE_TYPE: {
            return "double";
        }
        default: {
            return "untyped";
        }
    }
}

static void add_check_log(const char* str, Visitor* visitor) {
    MeanCheckLogger* log = (MeanCheckLogger*)cs_malloc(sizeof(MeanCheckLogger));
    log->next = NULL;
    log->log_str = (char*)cs_malloc(strlen(str) + 1);
    strcpy(log->log_str, str);
    MeanCheckLogger* p = NULL;
    if (((MeanVisitor*)visitor)->check_log == NULL) {
        ((MeanVisitor*)visitor)->check_log = log;
        return;
    } else {
        for (p = ((MeanVisitor*)visitor)->check_log; p->next; p = p->next)
            ;
        p->next = log;
    }
}

void show_mean_error(MeanVisitor* visitor) {
    MeanCheckLogger* p;
    for (p = visitor->check_log; p; p = p->next) {
        fprintf(stderr, "%s\n", p->log_str);
    }
}

static void enter_castexpr(Expression* expr, Visitor* visitor) {}
static void leave_castexpr(Expression* expr, Visitor* visitor) {}

static void enter_boolexpr(Expression* expr, Visitor* visitor) {}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_intexpr(Expression* expr, Visitor* visitor) {}
static void leave_intexpr(Expression* expr, Visitor* visitor) {
    expr->type = cs_create_type_specifier(CS_INT_TYPE);
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {
    expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
}

static void enter_identexpr(Expression* expr, Visitor* visitor) {}
static void leave_identexpr(Expression* expr, Visitor* visitor) {
    Declaration* decl = cs_search_decl_in_block();
    FunctionDeclaration* function = NULL;
    if (!decl) {
        decl = cs_search_decl_global(expr->u.identifier.name);
    }

    if (decl) {
        expr->type = decl->type;
        expr->u.identifier.u.declaration = decl;
        expr->u.identifier.is_function = CS_FALSE;
        return;
    }
    function = cs_search_function(expr->u.identifier.name);
    if (function) {
        expr->type = function->type;
        expr->u.identifier.u.function = function;
        expr->u.identifier.is_function = CS_TRUE;
        return;
    }

    char message[50];
    sprintf(message, "%d: Cannot find identifier %s", expr->line_number,
            expr->u.identifier.name);
    add_check_log(message, visitor);
}

static CS_Boolean check_nulltype_binary_expr(Expression* expr,
                                             Visitor* visitor) {
    Expression* left = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    if (left->type == NULL) {
        char message[100];
        sprintf(message, "%d: Cannot find left hand type", expr->line_number);
        add_check_log(message, visitor);
    }
    if (right->type == NULL) {
        char message[100];
        sprintf(message, "%d: Cannot find right hand type", expr->line_number);
        add_check_log(message, visitor);
    }
    if ((left->type == NULL) || (right->type == NULL)) {
        return CS_TRUE;
    }
    return CS_FALSE;
}

static void unacceptable_type_binary_expr(Expression* expr, Visitor* visitor) {
    Expression* left = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    char message[100];
    sprintf(
        message,
        "%d: type mismatch in arithmetic binary expression left:%s, right:%s",
        expr->line_number, get_type_name(left->type->basic_type),
        get_type_name(right->type->basic_type));

    add_check_log(message, visitor);
}

static void cast_arithmetic_binary_expr(Expression* expr, Visitor* visitor) {
    Expression* left = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;

    if (check_nulltype_binary_expr(expr, visitor)) {
        return;
    }

    if (cs_is_int(left->type) && cs_is_int(right->type)) {
        expr->type = cs_create_type_specifier(CS_INT_TYPE);
        return;
    } else if (cs_is_int(left->type) && cs_is_double(right->type)) {
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        cast->u.cast_expression.expr = left;
        expr->u.binary_expression.left = cast;
        expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
    } else if (cs_is_double(left->type) && cs_is_int(right->type)) {
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        cast->u.cast_expression.expr = right;
        expr->u.binary_expression.right = cast;
        expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
    } else if (cs_is_double(left->type) && cs_is_double(right->type)) {
        expr->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        return;
    } else {
        unacceptable_type_binary_expr(expr, visitor);
    }
}

/* arithmetic calculation*/
static void enter_addexpr(Expression* expr, Visitor* visitor) {}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
    cast_arithmetic_binary_expr(expr, visitor);
}
static void enter_subexpr(Expression* expr, Visitor* visitor) {}
static void leave_subexpr(Expression* expr, Visitor* visitor) {
    cast_arithmetic_binary_expr(expr, visitor);
}

static void enter_mulexpr(Expression* expr, Visitor* visitor) {}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
    cast_arithmetic_binary_expr(expr, visitor);
}
static void enter_divexpr(Expression* expr, Visitor* visitor) {}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
    cast_arithmetic_binary_expr(expr, visitor);
}
static void enter_modexpr(Expression* expr, Visitor* visitor) {}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
    cast_arithmetic_binary_expr(expr, visitor);
}

static void compare_type_check(Expression* expr, Visitor* visitor) {
    Expression* left = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;

    if (check_nulltype_binary_expr(expr, visitor)) {
        return;
    }
    if (cs_same_type(left->type, right->type)) {
        expr->type = cs_create_type_specifier(left->type->basic_type);
        return;
    } else if (cs_is_int(left->type) && cs_is_double(right->type)) {
        // cast left to double
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, left);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        expr->u.binary_expression.left = cast;
    } else if (cs_is_double(left->type) && cs_is_int(right->type)) {
        // cast right to double
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, right);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        expr->u.binary_expression.right = cast;
    } else {
        unacceptable_type_binary_expr(expr, visitor);
    }
    //    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_gtexpr(Expression* expr, Visitor* visitor) {}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
    compare_type_check(expr, visitor);
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
    compare_type_check(expr, visitor);
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {
    compare_type_check(expr, visitor);
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
    compare_type_check(expr, visitor);
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void compare_equality_type_check(Expression* expr, Visitor* visitor) {
    if (check_nulltype_binary_expr(expr, visitor)) {
        return;
    }
    Expression* left = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;
    char message[100];
    if (cs_is_boolean(left->type) && !cs_is_boolean(right->type)) {
        sprintf(
            message,
            "%d: type mismatch in equality binary expression left:%s, right:%s",
            expr->line_number, get_type_name(left->type->basic_type),
            get_type_name(right->type->basic_type));

        add_check_log(message, visitor);
    } else if (!cs_is_boolean(left->type) && cs_is_boolean(right->type)) {
        sprintf(
            message,
            "%d: type mismatch in equality binary expression left:%s, right:%s",
            expr->line_number, get_type_name(left->type->basic_type),
            get_type_name(right->type->basic_type));

        add_check_log(message, visitor);
    } else {
        compare_type_check(expr, visitor);
    }
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
    compare_equality_type_check(expr, visitor);
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void enter_neexpr(Expression* expr, Visitor* visitor) {}
static void leave_neexpr(Expression* expr, Visitor* visitor) {
    compare_equality_type_check(expr, visitor);
    expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
}

static void logical_type_check(Expression* expr, Visitor* visitor) {
    if (check_nulltype_binary_expr(expr, visitor)) {
        return;
    }

    Expression* left = expr->u.binary_expression.left;
    Expression* right = expr->u.binary_expression.right;

    if (cs_is_boolean(left->type) && cs_is_boolean(right->type)) {
        expr->type = cs_create_type_specifier(CS_BOOLEAN_TYPE);
    } else {
        char message[100];
        sprintf(message, "%d: && or || accept only boolean left:%s, right:%s",
                expr->line_number, get_type_name(left->type->basic_type),
                get_type_name(right->type->basic_type));

        add_check_log(message, visitor);
    }
}

static void enter_landexpr(Expression* expr, Visitor* visitor) {}
static void leave_landexpr(Expression* expr, Visitor* visitor) {
    logical_type_check(expr, visitor);
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
    logical_type_check(expr, visitor);
}

static void incdec_typecheck(Expression* expr, Visitor* visitor) {
    Expression* idexpr = expr->u.inc_dec;
    char message[100];
    if (idexpr->type == NULL) {
        sprintf(message, "%d: Cannot find ++ or -- type", expr->line_number);
        add_check_log(message, visitor);
        return;
    }

    ExpressionKind eKind = expr->u.inc_dec->kind;
    if (eKind == INT_EXPRESSION) {
        sprintf(message, "%d: Operand is Immediate data)", expr->line_number);
        add_check_log(message, visitor);
    }

    if (eKind == IDENTIFIER_EXPRESSION) {
        if (expr->u.inc_dec->u.identifier.is_function) {
            sprintf(message, "%d: Variable should not be a function)",
                    expr->line_number);
        }
    }

    if (idexpr->type->basic_type != CS_INT_TYPE) {
        sprintf(message, "%d: Operand is not INT type (%s)", expr->line_number,
                get_type_name(idexpr->type->basic_type));
        add_check_log(message, visitor);
        return;
    } else {
        expr->type = idexpr->type;
    }
}
static void enter_incexpr(Expression* expr, Visitor* visitor) {}
static void leave_incexpr(Expression* expr, Visitor* visitor) {
    incdec_typecheck(expr, visitor);
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
    incdec_typecheck(expr, visitor);
}

static void enter_minusexpr(Expression* expr, Visitor* visitor) {}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {
    char message[100];
    TypeSpecifier* type = expr->u.minus_expression->type;
    if (type == NULL) {
        sprintf(message, "%d: Cannot find - type", expr->line_number);
        add_check_log(message, visitor);
        return;
    }

    if ((type->basic_type != CS_INT_TYPE) &&
        (type->basic_type != CS_DOUBLE_TYPE)) {
        sprintf(message, "%d: Operand is not INT or DOUBLE type (%s)",
                expr->line_number, get_type_name(type->basic_type));
        add_check_log(message, visitor);
        return;
    } else {
        expr->type = type;
    }
}

static void enter_lognotexpr(Expression* expr, Visitor* visitor) {}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
    char message[100];
    TypeSpecifier* type = expr->u.logical_not_expression->type;

    if (type == NULL) {
        sprintf(message, "%d: Cannot find ! type", expr->line_number);
        add_check_log(message, visitor);
        return;
    }

    if (type->basic_type != CS_BOOLEAN_TYPE) {
        sprintf(message, "%d: Operand is not BOOLEAN type (%s)",
                expr->line_number, get_type_name(type->basic_type));
        add_check_log(message, visitor);
        return;
    } else {
        expr->type = type;
    }
}

static Expression* assignment_type_check(TypeSpecifier* ltype, Expression* expr,
                                         Visitor* visitor) {
    if (ltype == NULL) {
        char message[100];
        sprintf(message, "%d: Cannot find left hand type", expr->line_number);
        add_check_log(message, visitor);
        return expr;
    }
    if (expr->type == NULL) {
        char message[100];
        sprintf(message, "%d: Cannot find right hand type", expr->line_number);
        add_check_log(message, visitor);
        return expr;
    }

    if (ltype->basic_type == expr->type->basic_type) {
        return expr;
    } else if ((ltype->basic_type == CS_INT_TYPE) &&
               (expr->type->basic_type == CS_DOUBLE_TYPE)) {
        Expression* cast = cs_create_cast_expression(CS_DOUBLE_TO_INT, expr);
        cast->type = cs_create_type_specifier(CS_INT_TYPE);
        return cast;
    } else if ((ltype->basic_type == CS_DOUBLE_TYPE) &&
               (expr->type->basic_type == CS_INT_TYPE)) {
        Expression* cast = cs_create_cast_expression(CS_INT_TO_DOUBLE, expr);
        cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
        return cast;
    } else {
        char message[100];
        sprintf(message, "%d: assignment type error %s = %s", expr->line_number,
                get_type_name(ltype->basic_type),
                get_type_name(expr->type->basic_type));
        add_check_log(message, visitor);
    }
    return expr;
}
static void enter_assignexpr(Expression* expr, Visitor* visitor) {}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
    Expression* left = expr->u.assignment_expression.left;
    Expression* right = expr->u.assignment_expression.right;
    expr->u.assignment_expression.right =
        assignment_type_check(left->type, right, visitor);
    expr->type = left->type;
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
    FunctionCallExpression* f_expr = &expr->u.function_call_expression;
    FunctionDeclaration* func_dec = NULL;
    printf("type = %d\n", f_expr->function->kind);
    switch (f_expr->function->kind) {
        case IDENTIFIER_EXPRESSION: {
            //            printf("identifier!!!\n");
            //            printf("%s\n", f_expr->function->u.identifier.name);
            func_dec = cs_search_function(f_expr->function->u.identifier.name);
            break;
        }
        default: {
            fprintf(stderr, "this type cannot be the function %d\n",
                    f_expr->function->kind);
            exit(1);
        }
    }

    if (func_dec) {
        ParameterList* params = func_dec->param;
        ArgumentList* args = f_expr->argument;
        int params_count, args_count;
        for (params_count = 0; params; params = params->next, ++params_count)
            ;
        for (args_count = 0; args; args = args->next, ++args_count)
            ;
        //        printf("params_count = %d\n", params_count);
        //        printf("args_count   = %d\n", args_count);

        if (params_count != args_count) {
            fprintf(stderr, "argument count is not the same\n");
            char message[100];
            sprintf(message,
                    "%d: argument count mismatch in function call require:%d, "
                    "pass:%d",
                    expr->line_number, params_count, args_count);
            add_check_log(message, visitor);
        } else {
            for (params = func_dec->param, args = f_expr->argument; params;
                 params = params->next, args = args->next) {
                if (cs_same_type(params->type, args->expr->type)) {
                    // OK
                } else if (cs_is_int(params->type) &&
                           cs_is_double(args->expr->type)) {
                    Expression* cast =
                        cs_create_cast_expression(CS_DOUBLE_TO_INT, args->expr);
                    cast->type = cs_create_type_specifier(CS_INT_TYPE);
                    args->expr = cast;
                } else if (cs_is_double(params->type) &&
                           cs_is_int(args->expr->type)) {
                    Expression* cast =
                        cs_create_cast_expression(CS_INT_TO_DOUBLE, args->expr);
                    cast->type = cs_create_type_specifier(CS_DOUBLE_TYPE);
                    args->expr = cast;
                } else {
                    char message[100];
                    sprintf(message,
                            "%d: type mismatch in function call require:%s, "
                            "pass:%s",
                            expr->line_number,
                            get_type_name(params->type->basic_type),
                            get_type_name(args->expr->type->basic_type));
                    add_check_log(message, visitor);
                }
            }
        }
    }
    expr->type = expr->u.function_call_expression.function->type;
}

/* For statement */
static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
    //    fprintf(stderr, "enter exprstmt :\n");
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
    //    fprintf(stderr, "leave exprstmt\n");
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
    CS_Compiler* compiler = ((MeanVisitor*)visitor)->compiler;

    Declaration* decl = cs_search_decl_in_block();
    if (!decl) {
        decl = cs_search_decl_global(stmt->u.declaration_s->name);
    }

    if (decl) {
        char message[50];
        sprintf(message, "Already defind identifier %s",
                stmt->u.declaration_s->name);
        add_check_log(message, visitor);
        return;
    }

    compiler->decl_list =
        cs_chain_declaration(compiler->decl_list, stmt->u.declaration_s);
    //    fprintf(stderr, "enter declstmt\n");
}

static void leave_declstmt(Statement* stmt, Visitor* visitor) {
    //    fprintf(stderr, "leave declstmt\n");
    Declaration* decl = stmt->u.declaration_s;
    if (decl->initializer != NULL) {
        decl->initializer =
            assignment_type_check(decl->type, decl->initializer, visitor);
    }
}

MeanVisitor* create_mean_visitor() {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;

    MeanVisitor* visitor = MEM_malloc(sizeof(MeanVisitor));
    visitor->check_log = NULL;
    visitor->compiler = cs_get_current_compiler();
    if (visitor->compiler == NULL) {
        fprintf(stderr, "Compile is NULL\n");
        exit(1);
    }

    enter_expr_list =
        (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    leave_expr_list =
        (visit_expr*)MEM_malloc(sizeof(visit_expr) * EXPRESSION_KIND_PLUS_ONE);
    enter_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) *
                                              STATEMENT_TYPE_COUNT_PLUS_ONE);
    leave_stmt_list = (visit_stmt*)MEM_malloc(sizeof(visit_stmt) *
                                              STATEMENT_TYPE_COUNT_PLUS_ONE);

    enter_expr_list[BOOLEAN_EXPRESSION] = enter_boolexpr;
    enter_expr_list[INT_EXPRESSION] = enter_intexpr;
    enter_expr_list[DOUBLE_EXPRESSION] = enter_doubleexpr;
    enter_expr_list[IDENTIFIER_EXPRESSION] = enter_identexpr;
    enter_expr_list[ADD_EXPRESSION] = enter_addexpr;
    enter_expr_list[SUB_EXPRESSION] = enter_subexpr;
    enter_expr_list[MUL_EXPRESSION] = enter_mulexpr;
    enter_expr_list[DIV_EXPRESSION] = enter_divexpr;
    enter_expr_list[MOD_EXPRESSION] = enter_modexpr;
    enter_expr_list[GT_EXPRESSION] = enter_gtexpr;
    enter_expr_list[GE_EXPRESSION] = enter_geexpr;
    enter_expr_list[LT_EXPRESSION] = enter_ltexpr;
    enter_expr_list[LE_EXPRESSION] = enter_leexpr;
    enter_expr_list[EQ_EXPRESSION] = enter_eqexpr;
    enter_expr_list[NE_EXPRESSION] = enter_neexpr;
    enter_expr_list[LOGICAL_AND_EXPRESSION] = enter_landexpr;
    enter_expr_list[LOGICAL_OR_EXPRESSION] = enter_lorexpr;
    enter_expr_list[INCREMENT_EXPRESSION] = enter_incexpr;
    enter_expr_list[DECREMENT_EXPRESSION] = enter_decexpr;
    enter_expr_list[MINUS_EXPRESSION] = enter_minusexpr;
    enter_expr_list[LOGICAL_NOT_EXPRESSION] = enter_lognotexpr;
    enter_expr_list[ASSIGN_EXPRESSION] = enter_assignexpr;
    enter_expr_list[FUNCTION_CALL_EXPRESSION] = enter_funccallexpr;
    enter_expr_list[CAST_EXPRESSION] = enter_castexpr;

    enter_stmt_list[EXPRESSION_STATEMENT] = enter_exprstmt;
    enter_stmt_list[DECLARATION_STATEMENT] = enter_declstmt;

    leave_expr_list[BOOLEAN_EXPRESSION] = leave_boolexpr;
    leave_expr_list[INT_EXPRESSION] = leave_intexpr;
    leave_expr_list[DOUBLE_EXPRESSION] = leave_doubleexpr;
    leave_expr_list[IDENTIFIER_EXPRESSION] = leave_identexpr;
    leave_expr_list[ADD_EXPRESSION] = leave_addexpr;
    leave_expr_list[SUB_EXPRESSION] = leave_subexpr;
    leave_expr_list[MUL_EXPRESSION] = leave_mulexpr;
    leave_expr_list[DIV_EXPRESSION] = leave_divexpr;
    leave_expr_list[MOD_EXPRESSION] = leave_modexpr;
    leave_expr_list[GT_EXPRESSION] = leave_gtexpr;
    leave_expr_list[GE_EXPRESSION] = leave_geexpr;
    leave_expr_list[LT_EXPRESSION] = leave_ltexpr;
    leave_expr_list[LE_EXPRESSION] = leave_leexpr;
    leave_expr_list[EQ_EXPRESSION] = leave_eqexpr;
    leave_expr_list[NE_EXPRESSION] = leave_neexpr;
    leave_expr_list[LOGICAL_AND_EXPRESSION] = leave_landexpr;
    leave_expr_list[LOGICAL_OR_EXPRESSION] = leave_lorexpr;
    leave_expr_list[INCREMENT_EXPRESSION] = leave_incexpr;
    leave_expr_list[DECREMENT_EXPRESSION] = leave_decexpr;
    leave_expr_list[DECREMENT_EXPRESSION] = leave_decexpr;
    leave_expr_list[MINUS_EXPRESSION] = leave_minusexpr;
    leave_expr_list[LOGICAL_NOT_EXPRESSION] = leave_lognotexpr;
    leave_expr_list[ASSIGN_EXPRESSION] = leave_assignexpr;
    leave_expr_list[FUNCTION_CALL_EXPRESSION] = leave_funccallexpr;
    leave_expr_list[CAST_EXPRESSION] = leave_castexpr;

    leave_stmt_list[EXPRESSION_STATEMENT] = leave_exprstmt;
    leave_stmt_list[DECLARATION_STATEMENT] = leave_declstmt;

    ((Visitor*)visitor)->enter_expr_list = enter_expr_list;
    ((Visitor*)visitor)->leave_expr_list = leave_expr_list;
    ((Visitor*)visitor)->enter_stmt_list = enter_stmt_list;
    ((Visitor*)visitor)->leave_stmt_list = leave_stmt_list;
    ((Visitor*)visitor)->notify_expr_list = NULL;

    return visitor;
}