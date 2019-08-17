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
    ND_LE
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs, *rhs;
    int val;
};

// tokenizer.c
void *tokenize(char *p);
bool consume(char *op);
void expect(char *op);
int expect_number(void);

// parser.c
Node *expr(void);

// codegen.c
void code_gen(Node *node);