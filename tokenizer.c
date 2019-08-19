#include "9cc.h"

Token *token;
char *user_input;
LVar *locals;

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

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var != NULL; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

LVar *new_lvar(Token *tok) {
    LVar *var = calloc(1, sizeof(LVar));
    var->next = locals;
    var->name = tok->str;
    var->len = tok->len;
    var->offset = (locals == NULL) ? 8 : locals->offset + 8;
    return locals = var;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool double_symbol_op(char *p) {
    char *ops[4] = { "==", "!=", "<=", ">=" };
    for (int i = 0; i < 4; i++) if (!memcmp(p, ops[i], 2)) return true;
    return false;
}

bool single_symbol_op(char *p) {
    char ops[10] = "+-*/<>()=;";
    for (int i = 0; i < 10; i++) if (*p == ops[i]) return true;
    return false;
}

int ident_len(char *p) {
    int len = 0;
    while (p[len] != '\0' && !isspace(p[len]) && !double_symbol_op(p + len) &&
           !single_symbol_op(p + len) && !isdigit(p[len])) len++;
    return len;
}

int lvar_count(void) {
    int cnt = 0;
    for (LVar *var = locals; var != NULL; var = var->next) cnt++;
    return cnt;
}

void *tokenize(char *p) {
    user_input = p;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
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

        int len = ident_len(p);
        cur = new_token(TK_IDENT, cur, p, len);
        p += len;
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

bool consume_ident(int *offset) {
    if (token->kind != TK_IDENT) return false;

    LVar *var = find_lvar(token);
    if (var == NULL) var = new_lvar(token);
    token = token->next;
    *offset = var->offset;
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

bool at_eof(void) {
  return token->kind == TK_EOF;
}