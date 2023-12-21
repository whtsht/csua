
#include <stdio.h>
#include <stdlib.h>

#include "visitor.h"

static void traverse_expr_children(Expression* expr, Visitor* visitor);
static void traverse_stmt_children(Statement* stmt, Visitor* visitor);

void traverse_expr(Expression* expr, Visitor* visitor) {
    if (expr) {
        if (visitor->enter_expr_list[expr->kind] == NULL) {
            fprintf(stderr, "enter->type(%d) is null\n", expr->kind);
            exit(1);
        }

        visitor->enter_expr_list[expr->kind](expr, visitor);
        traverse_expr_children(expr, visitor);
        visitor->leave_expr_list[expr->kind](expr, visitor);
    }
}

void traverse_stmt(Statement* stmt, Visitor* visitor) {
    if (stmt) {
        if (visitor->enter_stmt_list[stmt->type] == NULL) {
            fprintf(stderr, "enter->type(%d) is null\n", stmt->type);
            exit(1);
        }
        visitor->enter_stmt_list[stmt->type](stmt, visitor);
        traverse_stmt_children(stmt, visitor);
        visitor->leave_stmt_list[stmt->type](stmt, visitor);
    }
}

static void traverse_stmt_children(Statement* stmt, Visitor* visitor) {
    switch (stmt->type) {
        case EXPRESSION_STATEMENT: {
            traverse_expr(stmt->u.expression_s, visitor);
            break;
        }
        case DECLARATION_STATEMENT: {
            traverse_expr(stmt->u.declaration_s->initializer, visitor);
            break;
        }
        case BLOCKOPERATION_STATEMENT: {
            // BLOCKOPERATION_STATEMENTに子が存在することはないのでトラバースしない
            // fprintf(stderr, "traversed: BLOCKOPERATION_STATEMENT\n");
            break;
        }
        case IF_STATEMENT: {
            traverse_expr(stmt->u.expression_s, visitor);
            break;
        }
        default: {
            fprintf(stderr, "No such stmt->type %d in traverse_stmt_children\n",
                    stmt->type);
        }
    }
}

static void traverse_expr_children(Expression* expr, Visitor* visitor) {
    switch (expr->kind) {
        case BOOLEAN_EXPRESSION:
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
            traverse_expr(expr->u.assignment_expression.right, visitor);
            if (visitor->notify_expr_list) {
                if (visitor->notify_expr_list[ASSIGN_EXPRESSION]) {
                    visitor->notify_expr_list[ASSIGN_EXPRESSION](expr, visitor);
                }
            }

            traverse_expr(expr->u.assignment_expression.left, visitor);
            break;
        }
        case CAST_EXPRESSION: {
            traverse_expr(expr->u.cast_expression.expr, visitor);
            break;
        }
        case FUNCTION_CALL_EXPRESSION: {
            //            printf("function call!\n");
            ArgumentList* args = expr->u.function_call_expression.argument;
            if (args) {
                for (; args; args = args->next) {
                    traverse_expr(args->expr, visitor);
                }
            }
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
            fprintf(stderr, "No such expr->kind %d in traverse_expr_children\n",
                    expr->kind);
    }
}
