#include "9cc.h"

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_STR,
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

typedef struct Str Str;
struct Str {
    Str *next;
    char *name;
    int len, sn;
};

long long str_sn = 0;
char *file_name;
char *user_input;
Token *token;
Var *globals;
Var *locals;
Str *strs;

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    char *line = loc, *end = loc;
    while (user_input < line && line[-1] != '\n') line--;
    while (*end != '\n') end++;

    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n') line_num++;

    int indent = fprintf(stderr, "%s:%d: ", file_name, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    int pos = loc - line + indent;
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

Type *get_array_base_type(Type *type) {
    while (type->t_kw == ARRAY) type =type->ptr_to;
    return type;
}

int get_type_size(Type *type) {
    switch (type->t_kw) {
    case INT:
        return 4;
    case CHAR:
        return 1;
    case PTR:
        return 8;
    case ARRAY:
        return type->array_size * get_type_size(type->ptr_to);
    }
    error("型が定義されていません");
}

Var *new_gvar(Type *type, char *name, int len) {
    Var *var = calloc(1, sizeof(Var));
    var->type = type;
    var->next = globals;
    var->name = name;
    var->len = len;
    return globals = var;
}

Var *new_lvar(Type *type, char *name, int len) {
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

Str *new_str(char *name, int len) {
    Str *str = calloc(1, sizeof(Str));
    str->next = strs;
    str->name = name;
    str->len = len;
    str->sn = str_sn++;
    return strs = str;
}

int get_strs(char ***name, int **len) {
    int i = 0;
    for (Str *str = strs; str != NULL; str = str->next, i++) {
        (*name)[i] = str->name;
        (*len)[i] = str->len;
    }
    return i;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool is_one_line_comment(char *p) {
    return !memcmp(p, "//", 2);
}

bool is_multi_line_comment(char *p) {
    return !memcmp(p, "/*", 2);
}

/** 予約語だった場合には文字数を、そうでないなら0を返す */
int is_reserved_word(char *p) {
    char *words[8] = { "return", "if", "else", "while", "for", "int", "char", "sizeof" };
    int word_sizes[8] = { 6, 2, 4, 5, 3, 3, 4, 6 };
    for (int i = 0; i < 8; i++) {
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
    char ops[17] = "+-*/<>()=;{},&[]";
    for (int i = 0; i < 17; i++) if (*p == ops[i]) return true;
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
    for (Var *var = locals; var != NULL; var = var->next)
        offset += get_type_size(var->type);
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

void *tokenize(char *f_name) {
    file_name = f_name;
    char *p = read_file(f_name);
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

        if (is_one_line_comment(p)) {
            while (*p != '\n') p++;
            continue;
        }

        if (is_multi_line_comment(p)) {
            char *q = strstr(p + 2, "*/");
            if (q == NULL) error_at(p, "コメントが閉じられていません");
            p = q + 2;
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

        if (*p == '"') {
            char *q = ++p;
            while (*q != '"') q++;
            cur = new_token(TK_STR, cur, p, q - p);
            p = q + 1;
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

bool consume(char *name) {
    if (token->kind != TK_RESERVED ||
        strlen(name) != token->len ||
        memcmp(token->str, name, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_func_call(char **name, int *len) {
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

bool consume_type(Type **type) {
    Type *t;
    if (consume("int")) t = new_type(INT);
    else if (consume("char")) t = new_type(CHAR);
    else return false;

    while (consume("*")) t = new_ptr_type(t);
    *type = t;
    return true;
}

bool consume_lvar(Type **type, int *offset) {
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

bool consume_str(int *sn) {
    if (token->kind != TK_STR) return false;
    Str *str = new_str(token->str, token->len);
    *sn = str->sn;
    token = token->next;
    return true;
}

bool consume_embedded_str(char **name, int *len) {
    if (token->kind != TK_STR) return false;
    *name = token->str;
    *len = token->len;
    token = token->next;
    return true;
}

void expect(char *name) {
    if (token->kind != TK_RESERVED ||
        strlen(name) != token->len ||
        memcmp(token->str, name, token->len))
        error_at(token->str, "\"%s\"ではありません", name);
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
    Type *type;
    if (consume("int")) type = new_type(INT);
    else {
        expect("char");
        type = new_type(CHAR);
    }

    while (consume("*")) type = new_ptr_type(type);
    return type;
}

void consume_array(Type **type) {
    int size[MAX_ARRAY_DIMENS], array_dimen = 0;
    while (consume("[")) {
        size[array_dimen++] = expect_number();
        expect("]");
    }
    for (int i = array_dimen - 1; i >= 0; i--)
        *type = new_array_type(*type, size[i]);
}

void define_local_variable(Type **type, int *offset) {
    if (token->kind != TK_IDENT)
        error_at(token->str, "変数ではありません");

    char *name = token->str;
    int len = token->len;
    token = token->next;

    consume_array(type);
    Var *var = new_lvar(*type, name, len);
    *offset = var->offset;
}

void define_global_variable(Type **type, char *name, int len) {
    consume_array(type);
    Var *var = new_gvar(*type, name, len);
}

bool at_eof(void) {
  return token->kind == TK_EOF;
}