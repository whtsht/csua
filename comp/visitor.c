#include "visitor.h"

#include <stdio.h>

#include "../memory/MEM.h"

static int depth = 0;

static void increment() { depth++; }

static void decrement() { depth--; }

void print_depth() {
    for (int i = 0; i < depth; ++i) {
        fprintf(stderr, "  ");
    }
}

/*
static char* get_type_name(CS_BasicType type) {
    switch(type) {
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
*/

static const char* get_cast_type_name(CS_CastType type) {
    switch (type) {
        case CS_INT_TO_DOUBLE: {
            return "int_to_double";
        }
        case CS_DOUBLE_TO_INT: {
            return "double_to_int";
        }
        default: {
            return "unknown cast";
        }
    }
}

static CS_BasicType get_type(Expression* expr) {
    if (expr->type) {
        return expr->type->basic_type;
    }
    return CS_BASIC_TYPE_PLUS_ONE;
}

static void enter_castexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter castexpr : %s\n",
            get_cast_type_name(expr->u.cast_expression.ctype));
    increment();
}
static void leave_castexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave castexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_boolexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter boolexpr : %d\n", expr->u.boolean_value);
    increment();
}
static void leave_boolexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave boolexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_intexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter intexpr : %d\n", expr->u.int_value);
    increment();
}
static void leave_intexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave intexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_doubleexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter doubleexpr : %f\n", expr->u.double_value);
    increment();
}
static void leave_doubleexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave doubleexpr(type:%s)\n",
            get_type_name(get_type(expr)));
}

static void enter_identexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter identifierexpr : %s\n", expr->u.identifier.name);
    increment();
}
static void leave_identexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave identifierexpr(type:%s)\n",
            get_type_name(get_type(expr)));
}

static void enter_addexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter addexpr : +\n");
    increment();
}
static void leave_addexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave addexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_subexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter subexpr : -\n");
    increment();
}
static void leave_subexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave subexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_mulexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter mulexpr : *\n");
    increment();
}
static void leave_mulexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave mulexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_divexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter divexpr : /\n");
    increment();
}
static void leave_divexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave divexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_modexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter modexpr : mod \n");
    increment();
}
static void leave_modexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave modexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_gtexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter gtexpr : > \n");
    increment();
}
static void leave_gtexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave gtexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_geexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter geexpr : >= \n");
    increment();
}
static void leave_geexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave geexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_ltexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter ltexpr : < \n");
    increment();
}
static void leave_ltexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave ltexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_leexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter leexpr : <= \n");
    increment();
}
static void leave_leexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave leexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_eqexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter eqexpr : == \n");
    increment();
}
static void leave_eqexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave eqexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_neexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter neexpr : != \n");
    increment();
}
static void leave_neexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave neexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_landexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter landexpr : && \n");
    increment();
}
static void leave_landexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave landexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_lorexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter lorexpr : || \n");
    increment();
}
static void leave_lorexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave lorexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_incexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter incexpr : ++ \n");
    increment();
}
static void leave_incexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave incexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_decexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter decexpr : -- \n");
    increment();
}
static void leave_decexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave decexpr(type:%s)\n", get_type_name(get_type(expr)));
}

static void enter_minusexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter minusexpr : - \n");
    increment();
}
static void leave_minusexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave minusexpr(type:%s)\n",
            get_type_name(get_type(expr)));
}

static void enter_lognotexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter lognotexpr : ! \n");
    increment();
}
static void leave_lognotexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave lognotexpr(type:%s)\n",
            get_type_name(get_type(expr)));
}

static void enter_assignexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter assignexpr : %d \n",
            expr->u.assignment_expression.aope);
    increment();
}
static void leave_assignexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave assignexpr(type:%s)\n",
            get_type_name(get_type(expr)));
}

static void enter_funccallexpr(Expression* expr, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter function call :\n");
    increment();
}
static void leave_funccallexpr(Expression* expr, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave function call(type:%s)\n",
            get_type_name(get_type(expr)));
}

/* For statement */
static void enter_exprstmt(Statement* stmt, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter exprstmt :\n");
    increment();
}
static void leave_exprstmt(Statement* stmt, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave exprstmt\n");
}

static void enter_declstmt(Statement* stmt, Visitor* visitor) {
    print_depth();
    fprintf(stderr, "enter declstmt name=%s, type=%s:\n",
            stmt->u.declaration_s->name,
            get_type_name(stmt->u.declaration_s->type->basic_type));

    increment();
}

static void leave_declstmt(Statement* stmt, Visitor* visitor) {
    decrement();
    print_depth();
    fprintf(stderr, "leave declstmt\n");
}

static void enter_blkopstmt(Statement* stmt, Visitor* visitor){
    print_depth();
    fprintf(stderr, "enter blkopstmt\n");
    increment();
}
static void leave_blkopstmt(Statement* stmt, Visitor* visitor){
    decrement();
    print_depth();
    fprintf(stderr, "leave blkopstmt\n");
}

Visitor* create_treeview_visitor() {
    visit_expr* enter_expr_list;
    visit_expr* leave_expr_list;
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;

    Visitor* visitor = MEM_malloc(sizeof(Visitor));
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
    enter_stmt_list[BLOCKOPERATION_STATEMENT] = enter_blkopstmt;

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
    leave_stmt_list[BLOCKOPERATION_STATEMENT] = leave_blkopstmt;

    visitor->enter_expr_list = enter_expr_list;
    visitor->leave_expr_list = leave_expr_list;
    visitor->enter_stmt_list = enter_stmt_list;
    visitor->leave_stmt_list = leave_stmt_list;
    visitor->notify_expr_list = NULL;

    return visitor;
}

void delete_visitor(Visitor* visitor) {
    MEM_free(visitor->enter_expr_list);
    MEM_free(visitor->leave_expr_list);
    MEM_free(visitor->enter_stmt_list);
    MEM_free(visitor->leave_stmt_list);
    if (visitor->notify_expr_list) {
        MEM_free(visitor->notify_expr_list);
    }
    MEM_free(visitor);
}
