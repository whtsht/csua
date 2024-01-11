#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csua.h"
#include "keyword.h"
#include "y.tab.h"

#define ISASCII(c) isascii((unsigned char)(c))
#define ISALNUM(c) (ISASCII(c) && isalnum((unsigned char)(c)))
#define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#define is_identchar(c) \
    (SIGN_EXTEND_CHAR(c) != -1 && (ISALNUM(c) || (c) == '_'))

extern struct OPE *in_word_set(char *, unsigned int);

#define BUFSIZE 5

FILE *yyin = NULL;

static uint8_t *buffer = NULL;
static uint32_t ptr = 0;
static uint32_t limit = 0;

char *yytext;
static uint32_t ytp = 0;
static uint32_t yt_max = 0;

/* for debug*/
int current_line = 1;
static void increment_line() {
    CS_Compiler *compiler = cs_get_current_compiler();
    if (compiler) {
        compiler->current_line++;
    } else {
        current_line++;
    }
}

int get_current_line() { return current_line; }

static void real_read() {
    if (buffer == NULL) {
        buffer = (uint8_t *)malloc(BUFSIZE);
    }

    if (yytext == NULL) {
        ytp = 0;
        yytext = (char *)malloc(BUFSIZE);
        yt_max += BUFSIZE;
    }

    ptr = 0;
    limit = fread(buffer, 1, BUFSIZE, yyin);
}

static uint8_t read() {
    if (ptr == limit) real_read();
    if (limit == 0) {
        return EOF;
    }
    return buffer[ptr++];
}

static void addText(char c) {
    if (ytp == (yt_max - 1)) {
        yt_max += BUFSIZE;
        yytext = (char *)realloc(yytext, yt_max);
    }
    yytext[ytp++] = c;
    yytext[ytp] = 0;
}

// test function
// static void showText() {
//     for (int i = 0; i < ytp; ++i) {
//         printf("%d ", (int)yytext[i]);
//     }
//     printf("\n");
// }

static void pushback() { --ptr; }

static void error() {
    fprintf(stderr, "cannot understand character: %s\n", yytext);
    exit(1);
}

int yylex() {
    char c;
    //    c = read();
    ytp = 0;
    /*
    for (int i = 0; i < 5; ++i) {
        c = read();
        printf("c = %c\n", c);
    }
    c = read();
    printf("c = %c\n", c);
    printf("ptr = %d\n", (int)ptr);
    pushback();
    c = read();
    printf("c = %c\n", c);
    printf("ptr = %d\n", (int)ptr);
    */

retry:
    switch (c = read()) {
        case '#': {  // skip comment
            while ((c = read()) != '\n')
                ;
            increment_line();
            //            printf("skip\n");
            goto retry;
        }
        case ' ':
        case '\t': {  // skip space and tab
            goto retry;
        }
        case '\r':
        case '\n': {  // ignore return and count the number
            increment_line();
            goto retry;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            addText(c);
            while (isdigit(c = read())) {
                addText(c);
            }
            if (c == '.') {  // double value
                addText(c);
                uint8_t dbl_flg = 0;
                while (isdigit(c = read())) {
                    dbl_flg = 1;
                    addText(c);
                }
                if (dbl_flg) {
                    pushback();
                    double d_value;
                    sscanf(yytext, "%lf", &d_value);
                    //                    printf("double_value = %lf\n",
                    //                    d_value);
                    yylval.dv = d_value;
                    return DOUBLE_LITERAL;
                } else {  // error
                    fprintf(stderr, "double error\n");
                    exit(1);
                }
            } else {  // int value
                pushback();
                int i_value;
                sscanf(yytext, "%d", &i_value);
                //                printf("int_value = %d\n", i_value);
                yylval.iv = i_value;
                return INT_LITERAL;
            }
            break;
        }
        case ';': {
            return SEMICOLON;
        }
        case '(': {
            return LP;
        }
        case ')': {
            return RP;
        }
        case '{': {
            return LC;
        }
        case '}': {
            return RC;
        }
        case ',': {
            return COMMA;
        }
        case '&': {
            addText(c);
            if ((c = read()) == '&') {
                return LOGICAL_AND;
            } else {
                addText(c);
                error();
            }
        }
        case '|': {
            addText(c);
            if ((c = read()) == '|') {
                return LOGICAL_OR;
            } else {
                addText(c);
                error();
            }
        }
        case '=': {
            if ((c = read()) == '=') {
                return EQ;
            } else {
                pushback();
                return ASSIGN_T;
            }
        }
        case '!': {
            if ((c = read()) == '=') {
                return NE;
            } else {
                pushback();
                return EXCLAMATION;
            }
        }
        case '>': {
            if ((c = read()) == '=') {
                return GE;
            } else {
                pushback();
                return GT;
            }
        }
        case '<': {
            if ((c = read()) == '=') {
                return LE;
            } else {
                pushback();
                return LT;
            }
        }
        case '+': {
            c = read();
            if (c == '+') {
                return INCREMENT;
            } else if (c == '=') {
                return ADD_ASSIGN_T;
            } else {
                pushback();
                return ADD;
            }
        }
        case '-': {
            c = read();
            if (c == '-') {
                return DECREMENT;
            } else if (c == '=') {
                return SUB_ASSIGN_T;
            } else {
                pushback();
                return SUB;
            }
        }
        case '*': {
            if ((c = read()) == '=') {
                return MUL_ASSIGN_T;
            } else {
                pushback();
                return MUL;
            }
        }
        case '/': {
            if ((c = read()) == '=') {
                return DIV_ASSIGN_T;
            } else {
                pushback();
                return DIV;
            }
        }
        case '%': {
            if ((c = read()) == '=') {
                return MOD_ASSIGN_T;
            } else {
                pushback();
                return MOD;
            }
        }
        case '.': {
            return DOT;
        }
        case EOF: {
            return EOF;
        }
        default: {
            break;
        }
    }

    while (is_identchar(c)) {
        addText(c);
        c = read();
    }
    pushback();
    struct OPE *op = in_word_set(yytext, strlen(yytext));
    if (op != NULL) {
        //      printf("not null -> %d\n", op->type);
        //        printf("not null -> %s\n", op->name);
        return op->type;
    }

    //    yylval.name = yytext;
    yylval.name = cs_create_identifier(yytext);
    return IDENTIFIER;

    return EOF;
}
