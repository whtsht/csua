#include <stdio.h>
#include <stdlib.h>

#include "csua.h"
#include "y.tab.h"

int main(void) {
    extern FILE *yyin;
    extern int yylex();
    yyin = fopen("tests/test.cs", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }

    int t_type;

    while (1) {
        t_type = yylex();

        switch (t_type) {
            case INT_LITERAL: {
                printf("INT_LITERAL: %d\n", yylval.iv);
                break;
            }
            case DOUBLE_LITERAL: {
                printf("DOUBLE_LITERAL: %lf\n", yylval.dv);
                break;
            }
            case SEMICOLON: {
                printf("SEMICOLON:\n");
                break;
            }
            case LP: {
                printf("LP\n");
                break;
            }
            case RP: {
                printf("RP\n");
                break;
            }
            case LC: {
                printf("LC\n");
                break;
            }
            case RC: {
                printf("RC\n");
                break;
            }
            case COMMA: {
                printf("COMMA\n");
                break;
            }
            case LOGICAL_AND: {
                printf("LOGICAL_AND\n");
                break;
            }
            case LOGICAL_OR: {
                printf("LOGICAL_OR\n");
                break;
            }
            case EQ: {
                printf("EQ\n");
                break;
            }
            case ASSIGN_T: {
                printf("ASSIGN_T\n");
                break;
            }
            case NE: {
                printf("NE\n");
                break;
            }
            case GE: {
                printf("GE\n");
                break;
            }
            case GT: {
                printf("GT\n");
                break;
            }
            case LE: {
                printf("LE\n");
                break;
            }
            case LT: {
                printf("LT\n");
                break;
            }
            case COLON: {
                printf("COLON\n");
                break;
            }
            case ADD: {
                printf("ADD\n");
                break;
            }
            case SUB: {
                printf("SUB\n");
                break;
            }
            case MUL: {
                printf("MUL\n");
                break;
            }
            case DIV: {
                printf("DIV\n");
                break;
            }
            case MOD: {
                printf("MOD\n");
                break;
            }
            case IF: {
                printf("IF\n");
                break;
            }
            case ELSE: {
                printf("ELSE\n");
                break;
            }
            case ELSIF: {
                printf("ELSIF\n");
                break;
            }
            case WHILE: {
                printf("WHILE\n");
                break;
            }
            case FOR: {
                printf("FOR\n");
                break;
            }
            case RETURN: {
                printf("RETURN\n");
                break;
            }
            case BREAK: {
                printf("BREAK\n");
                break;
            }
            case CONTINUE: {
                printf("CONTINUE\n");
                break;
            }
            case TRUE_T: {
                printf("TRUE_T\n");
                break;
            }
            case FALSE_T: {
                printf("FALSE_T\n");
                break;
            }
            case BOOLEAN_T: {
                printf("BOOLEAN_T\n");
                break;
            }
            case INT_T: {
                printf("INT_T\n");
                break;
            }
            case DOUBLE_T: {
                printf("DOUBLE_T\n");
                break;
            }
            case STRING_T: {
                printf("STRING_T\n");
                break;
            }
            case IDENTIFIER: {
                printf("IDENTIFIER: %s\n", yylval.name);
                break;
            }
            case EOF: {
                printf("END OF FILE\n");
                //                printf("line = %d\n", get_current_line());

                exit(1);
                break;
            }
        }
    }

    return 0;
}
