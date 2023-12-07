#ifndef _SCANTEST_H_
#define _SCANTEST_H_

#define INT_LITERAL 100
#define DOUBLE_LITERAL 101
#define LP 102
#define RP 103
#define LC 104
#define RC 105
#define COMMA 106
#define LOGICAL_AND 107
#define LOGICAL_OR 108
#define EQ 109
#define ASSIGN_T 110
#define NE 111
#define GE 112
#define GT 113
#define LE 114
#define LT 115
#define SEMICOLON 116
#define COLON 117
#define ADD 118
#define SUB 119
#define MUL 120
#define DIV 121
#define MOD 122
#define ADD_ASSIGN_T 123
#define SUB_ASSIGN_T 124
#define MUL_ASSIGN_T 125
#define DIV_ASSIGN_T 126
#define MOD_ASSIGN_T 127
#define INCREMENT 128
#define DECREMENT 129
#define EXCLAMATION 130
#define DOT 131
#define IDENTIFIER 132

// reserved keyword
#define IF 200
#define ELSE 201
#define ELSIF 202
#define WHILE 203
#define FOR 204
#define RETURN 205
#define BREAK 206
#define CONTINUE 207
#define TRUE 208
#define FALSE 209
#define BOOLEAN_T 210
#define INT_T 211
#define DOUBLE_T 212
#define STRING_T 213

typedef union {
    int iv;
    double dv;
    char *name;
} YYSTYPE;

YYSTYPE yylval;

#endif