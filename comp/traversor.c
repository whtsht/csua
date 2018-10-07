
#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"

static void traverse_expr_children(Expression* expr, Visitor *visitor);

void traverse_expr(Expression* expr, Visitor* visitor) {
    if (expr) {
        if (visitor->enter_expr_list[expr->kind] == NULL) {
            fprintf(stderr, "enter->type(%d) it null\n", expr->kind);
            exit(1);
        }

        visitor->enter_expr_list[expr->kind](expr);
        traverse_expr_children(expr, visitor);
        visitor->leave_expr_list[expr->kind](expr);

    }    
}

static void traverse_expr_children(Expression* expr, Visitor *visitor) {
    switch(expr->kind) {
        case IDENTIFIER_EXPRESSION:
        case DOUBLE_EXPRESSION:
        case INT_EXPRESSION: {
            break;
        }

        case INCREMENT_EXPRESSION:
        case DECREMENT_EXPRESSION: {
            traverse_expr(expr->u.inc_dec, visitor);
            break;
        }
        case MINUS_EXPRESSION: {
            traverse_expr(expr->u.minus_expression, visitor);
            break;
        }
        case LOGICAL_NOT_EXPRESSION: {
            traverse_expr(expr->u.logical_not_expression, visitor);
            break;
        }
        case ASSIGN_EXPRESSION: {
            traverse_expr(expr->u.assignment_expression.left, visitor);
            traverse_expr(expr->u.assignment_expression.right, visitor);
            break;
        }
        case FUNCTION_CALL_EXPRESSION: {
            traverse_expr(expr->u.function_call_expression.function, visitor);
            break;
        }
        case LOGICAL_AND_EXPRESSION:
        case LOGICAL_OR_EXPRESSION:            
        case LT_EXPRESSION:
        case LE_EXPRESSION:
        case GT_EXPRESSION:
        case GE_EXPRESSION:
        case EQ_EXPRESSION:
        case NE_EXPRESSION:            
        case MOD_EXPRESSION:
        case DIV_EXPRESSION:
        case MUL_EXPRESSION:
        case SUB_EXPRESSION:
        case ADD_EXPRESSION: {
            if (expr->u.binary_expression.left) {
                traverse_expr(expr->u.binary_expression.left, visitor);
            }
            if (expr->u.binary_expression.right) {
                traverse_expr(expr->u.binary_expression.right, visitor);
            }
            break;
        }
        default:
            fprintf(stderr, "No such expr->kind %d\n", expr->kind);
    }
}