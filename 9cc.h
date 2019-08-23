#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_GT,
    ND_GE,
    ND_LT,
    ND_LE,
    ND_ASSIGN,
    ND_LVAR,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_RETURN,
    ND_BLOCK,
    ND_FUNC,
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    // ND_BLOCKやND_FUNCの時にnextを使う
    Node *lhs, *rhs, *next;
    int val, offset;
    char *name;
    int len;
};

typedef struct LVar LVar;
struct LVar {
    LVar *next;
    char *name;
    int len, offset;
};

// tokenizer.c
int lvar_count(void);
void *tokenize(char *p);
bool consume(char *op);
bool consume_func(char **name, int *len);
bool consume_ident(int *offset);
void expect(char *op);
int expect_number(void);
bool at_eof(void);

// parser.c
void program(Node **nodes);

// codegen.c
void code_gen(Node **nodes);

// util.c
void error(char *fmt, ...);