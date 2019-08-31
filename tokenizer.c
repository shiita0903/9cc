#include "9cc.h"

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

typedef struct Var Var;
struct Var {
    Var *next;
    Type *type;
    char *name;
    int len, offset;
};

Token *token;
char *user_input;
Var *globals;
Var *locals;

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Type *new_type(TypeKeyword t_kw) {
    Type *type = calloc(1, sizeof(Type));
    type->t_kw = t_kw;
}

Type *new_ptr_type(Type *type) {
    Type *t = new_type(PTR);
    t->ptr_to = type;
    return t;
}

Type *new_array_type(Type *type, int size) {
    Type *t = new_type(ARRAY);
    t->ptr_to = type;
    t->array_size = size;
    return t;
}

int get_type_size(Type *type) {
    switch (type->t_kw) {
    case INT:
        return 8;   // TODO: intは32bitに対応する必要がある
    case PTR:
        return 8;
    case ARRAY:
        return type->array_size * get_type_size(type->ptr_to);
    }
    error("型が定義されていません");
}

Var *new_gvar(char *name, int len, Type *type, int size) {
    Var *var = calloc(1, sizeof(Var));
    var->type = type;
    var->next = globals;
    var->name = name;
    var->len = len;
    return globals = var;
}

Var *new_lvar(char *name, int len, Type *type, int size) {
    Var *var = calloc(1, sizeof(Var));
    var->type = type;
    var->next = locals;
    var->name = name;
    var->len = len;
    var->offset = (locals == NULL) ? 0 : locals->offset;
    var->offset += get_type_size(var->type);
    return locals = var;
}

Var *find_variable(Var *vars, char *name, int len) {
    for (Var *var = vars; var != NULL; var = var->next)
        if (!memcmp(name, var->name, len))
            return var;
    return NULL;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

/** 予約語だった場合には文字数を、そうでないなら0を返す */
int is_reserved_word(char *p) {
    char *words[7] = { "return", "if", "else", "while", "for", "int", "sizeof" };
    int word_sizes[7] = { 6, 2, 4, 5, 3, 3, 6 };
    for (int i = 0; i < 7; i++) {
        char *w = words[i];
        int ws = word_sizes[i];
        if (!memcmp(p, w, ws) && !isalnum(p[ws]) && p[ws] != '_')
            return ws;
    }
    return 0;
}

bool double_symbol_op(char *p) {
    char *ops[4] = { "==", "!=", "<=", ">=" };
    for (int i = 0; i < 4; i++) if (!memcmp(p, ops[i], 2)) return true;
    return false;
}

bool single_symbol_op(char *p) {
    char ops[16] = "+-*/<>()=;{},&[]";
    for (int i = 0; i < 16; i++) if (*p == ops[i]) return true;
    return false;
}

int ident_len(char *p) {
    int len = 0;
    while (p[len] != '\0' && !isspace(p[len]) &&
           !double_symbol_op(p + len)  && !single_symbol_op(p + len)) len++;
    return len;
}

int lvar_offset(void) {
    int offset = 0;
    for (Var *var = locals; var != NULL; var = var->next) {
        // TODO: intは32bitに対応する必要がある
        if (var->type->t_kw == ARRAY) offset += 8 * var->type->array_size;
        else offset += 8;
    }
    return offset;
}

void clear_lvar(void) {
    Var *var = locals;
    while (var != NULL) {
        Var *tmp = var->next;
        free(var);
        var = tmp;
    }
    locals = NULL;
}

void *tokenize(char *p) {
    user_input = p;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    int len;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if ((len = is_reserved_word(p)) > 0) {
            cur = new_token(TK_RESERVED, cur, p, len);
            p += len;
            continue;
        }

        if (double_symbol_op(p)) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (single_symbol_op(p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 1);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        if ((len = ident_len(p)) > 0) {
            cur = new_token(TK_IDENT, cur, p, len);
            p += len;
            continue;
        }

        error("トークナイズに失敗しました");
    }

    new_token(TK_EOF, cur, p, 1);
    token = head.next;
}

bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_func(char **name, int *len) {
    if (token->kind != TK_IDENT) return false;

    if (token->next->kind == TK_RESERVED &&
        token->next->str[0] == '(') {
        *name = token->str;
        *len = token->len;
    }
    else return false;

    token = token->next;
    return true;
}

bool consume_lvar(int *offset, Type **type) {
    if (token->kind != TK_IDENT) return false;
    Var *var = find_variable(locals, token->str, token->len);
    if (var == NULL) return false;

    *offset = var->offset;
    *type = var->type;
    token = token->next;
    return true;
}

bool consume_gvar(Type **type, char **name, int *len) {
    if (token->kind != TK_IDENT) return false;
    Var *var = find_variable(globals, token->str, token->len);
    if (var == NULL) return false;

    *type = var->type;
    *name = var->name;
    *len = var->len;
    token = token->next;
    return true;
}

bool consume_type(Type **type) {
    if (!consume("int")) return false;

    Type *t = new_type(INT);
    while (consume("*")) t = new_ptr_type(t);
    *type = t;
    return true;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "\"%s\"ではありません", op);
    token = token->next;
}

int expect_number(void) {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

void expect_ident(char **name, int *len) {
    if (token->kind != TK_IDENT) error_at(token->str, "識別子ではありません");
    *name = token->str;
    *len = token->len;
    token = token->next;
}

Type *expect_type() {
    expect("int");

    Type *type = new_type(INT);
    while (consume("*")) type = new_ptr_type(type);
    return type;
}

void define_local_variable(int *offset, Type **type) {
    if (token->kind != TK_IDENT)
        error_at(token->str, "変数ではありません");

    char *name = token->str;
    int len = token->len, size = 0;
    token = token->next;
    if (consume("[")) {
        size = expect_number();
        expect("]");
    }
    if (size > 0) *type = new_array_type(*type, size);

    Var *var = new_lvar(name, len, *type, size);
    if (offset != NULL) *offset = var->offset;
}

void define_global_variable(Type **type, char *name, int len) {
    int size = 0;
    // TODO: 2次元配列以上の実装が必要
    if (consume("[")) {
        size = expect_number();
        expect("]");
    }
    if (size > 0) *type = new_array_type(*type, size);

    Var *var = new_gvar(name, len, *type, size);
}

bool at_eof(void) {
  return token->kind == TK_EOF;
}