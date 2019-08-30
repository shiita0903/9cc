#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_FUNC_COUNT 1000     // 定義可能な関数の数

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
    ND_FUNC_DEF,
    ND_ADDR,
    ND_DEREF
} NodeKind;

typedef enum {
    INT,
    PTR,
    ARRAY
} TypeKeyword;

typedef struct Type Type;
struct Type {
    TypeKeyword t_kw;
    Type *ptr_to;
    size_t array_size;
};

typedef struct Node Node;
struct Node {
    NodeKind kind;
    // ND_BLOCKやND_FUNCの時にnextを使う
    Node *lhs, *rhs, *next;
    int val, offset;
    char *name;
    int len;
    Type *type;
};

// tokenizer.c
int lvar_offset(void);
void clear_lvar(void);
void *tokenize(char *p);
bool consume(char *op);
bool consume_func(char **name, int *len);
bool consume_ident(int *offset, Type **type);
bool consume_type(Type **type);
void expect(char *op);
int expect_number(void);
void expect_func_name(char **name, int *len);
void expect_func_def(char **name, int *len);
void define_local_variable(int *offset, Type **type);
bool at_eof(void);

// parser.c
bool is_pointer(Node *n);
void program(Node **nodes);

// codegen.c
void code_gen(Node **nodes);

// util.c
void error(char *fmt, ...);