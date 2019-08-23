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

/** 予約語だった場合には文字数を、そうでないなら0を返す */
int is_reserved_word(char *p) {
    char *words[5] = { "return", "if", "else", "while", "for" };
    int word_sizes[5] = { 6, 2, 4, 5, 3};
    for (int i = 0; i < 5; i++) {
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
    char ops[13] = "+-*/<>()=;{},";
    for (int i = 0; i < 13; i++) if (*p == ops[i]) return true;
    return false;
}

int ident_len(char *p) {
    int len = 0;
    while (p[len] != '\0' && !isspace(p[len]) &&
           !double_symbol_op(p + len)  && !single_symbol_op(p + len)) len++;
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