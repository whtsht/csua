#include <stdio.h>
#include <stdlib.h>
#include "scantest.h"

#define ERR -1
#define OK 1

extern FILE *yyin;
extern int yylex();

static int t_type;

static int EFLG = OK;

static int depth = 0;

static void increment() {
    depth++;
}

static void decrement() {
    depth--;
}

static void debug(const char* str) {
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("%s\n", str);
}

static void token_test() {    

    while (1) {
        t_type = yylex();
        switch (t_type) {
            case SEMICOLON: {
                printf("SEMICOLON(;)\n");
                break;
            }
            case INT_LITERAL: {
                printf("INT: %d\n", yylval.iv);
                break;            
            }
            case ADD: {
                printf("ADD(+)\n");
                break;
            }
            case SUB: {
                printf("SUB(-)\n");
                break;
            }
            case MUL: {
                printf("MUL(*)\n");
                break;
            }
            case DIV: {
                printf("DIV(/)\n");
                break;
            }
            case LP: {
                printf("(\n");
                break;
            }
            case RP: {
                printf(")\n");
                break;
            }
            case EOF: {
                printf("END OF FILE\n");
                exit(1);
                break;
            }
        }
    }    
}

static void expression();

static void factor() {
    increment();
    debug("factor");
    //printf("factor\n");

    switch (t_type) {
        case INT_LITERAL: {
            t_type = yylex();
            break;
        }
        case LP: {
            t_type = yylex();
            expression();
            if (t_type == RP) {
                t_type = yylex();
            } else {
                EFLG = ERR;
            }
            break;
        }
        default: {            
            EFLG = ERR;
            break;
        }
    }
    decrement();
    /*
    if (t_type == INT_LITERAL) {        
        t_type = yylex();
        return;
    }
    */
    
}

static void term() {
    increment();
    //printf("term\n");
    debug("term");
    factor();
    if (t_type == MUL | t_type == DIV) {
        //printf("* | /\n");
        debug("* | /");
        t_type = yylex();
        factor();
    }
    decrement();
}

static void expression() {
    increment();
    //printf("expression\n");
    debug("expression");
    term();
    if (t_type == ADD | t_type == DIV) {
        //printf("+ | -\n");
        debug("+ | -");
        t_type = yylex();
        term();
    }

    decrement();
}


static void statement() {
    printf("statement\n");
    expression();
    //t_type = yylex();
    if (t_type == SEMICOLON) {
        //printf("semicolon\n");
        return;
    } 
    t_type = yylex();
    if (t_type == EOF) {
        printf("end of input\n");
        exit(1);
    }
    
    EFLG = ERR;
}


int main(void) {
    
    yyin = fopen("tests/calc.cs", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }
    //printf("Hello World\n");
    debug("Hello World");
    //token_test();
    
    t_type = yylex();
    statement();

    if (EFLG == ERR) {
        printf("parse error\n");
    } else {
        printf("parse success\n");
    }
    
}