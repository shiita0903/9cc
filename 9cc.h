#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_NODE_COUNT 1000     // 定義可能な関数とグローバル変数の数
#define MAX_STR_COUNT 10000     // 定義可能な文字列リテラルの数
#define MAX_ARRAY_DIMENS 10     // 定義可能な多次元配列の次元数

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_STR,
    ND_EQ,
    ND_NE,
    ND_GT,
    ND_GE,
    ND_LT,
    ND_LE,
    ND_ASSIGN,
    ND_LVAR,
    ND_GVAR,
    ND_GVAR_DEF,
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
    CHAR,
    PTR,
    ARRAY
} TypeKeyword;

typedef struct Type Type;
struct Type {
    TypeKeyword t_kw;
    Type *ptr_to;
    int array_size;
};

typedef struct Node Node;
struct Node {
    NodeKind kind;
    // ND_BLOCKやND_FUNCの時にnextを使う
    Node *lhs, *rhs, *next;
    Type *type;
    int val, offset;
    char *name;
    int len;
};

// tokenizer.c
Type *new_type(TypeKeyword t_kw);
Type *new_ptr_type(Type *type);
int get_type_size(Type *type);
int get_strs(char ***name, int **len);
int lvar_offset(void);
void clear_lvar(void);
void *tokenize(char *p);
bool consume(char *name);
bool consume_func_call(char **name, int *len);
bool consume_type(Type **type);
bool consume_lvar(Type **type, int *offset);
bool consume_gvar(Type **type, char **name, int *len);
bool consume_str(int *sn);
void expect(char *name);
int expect_number(void);
void expect_ident(char **name, int *len);
Type *expect_type();
void define_local_variable(Type **type, int *offset);
void define_global_variable(Type **type, char *name, int len);
bool at_eof(void);

// parser.c
Type *get_node_type(Node *node);
void program(Node **nodes);

// codegen.c
void code_gen(Node **nodes);

// util.c
void error(char *fmt, ...);
char *read_file(char *path);
void print_type(Type *type);
void print_node(Node *node);