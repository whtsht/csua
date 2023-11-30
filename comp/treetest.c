#include <stdio.h>
#include <stdlib.h>

#include "../memory/MEM.h"
#include "visitor.h"

int main(void) {
    Expression* left = cs_create_int_expression(10);
    Expression* right = cs_create_int_expression(20);

    Expression* left2 = cs_create_int_expression(100);
    Expression* right2 = cs_create_int_expression(200);

    Expression* mul_expr =
        cs_create_binary_expression(MUL_EXPRESSION, left2, right2);

    Expression* add_expr =
        cs_create_binary_expression(ADD_EXPRESSION, left, mul_expr);

    Expression* ident_expr = cs_create_identifier_expression("abc");

    Expression* inc_expr =
        cs_create_inc_dec_expression(right, INCREMENT_EXPRESSION);
    Expression* minus_expr = cs_create_minus_expression(right);

    Expression* assign_expr =
        cs_create_assignment_expression(ident_expr, ASSIGN, right2);

    Expression* funccall_expr =
        cs_create_function_call_expression(ident_expr, NULL);

    Visitor* visitor = create_treeview_visitor();
    traverse_expr(add_expr, visitor);
    traverse_expr(ident_expr, visitor);
    traverse_expr(inc_expr, visitor);
    traverse_expr(minus_expr, visitor);
    traverse_expr(assign_expr, visitor);
    traverse_expr(funccall_expr, visitor);

    delete_visitor(visitor);

    delete_storage();
    MEM_dump_memory();

    return 0;
}
