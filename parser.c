#include "9cc.h"

Node *fragment(void);
Node *global(Type *type, char *name, int len);
Node *func(Type *type, char *name, int len);
Node *stmt(void);
Node *expr(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *factor(void);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->type = new_type(INT);
    node->val = val;
    return node;
}

Node *new_node_ident(NodeKind kind, Type *type, char *name, int len) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->type = type;
    node->name = name;
    node->len = len;
    return node;
}

Node *new_node_lvar(Type *type, int offset) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->type = type;
    node->offset = offset;
    return node;
}

bool is_pointer_t_kw(TypeKeyword t_kw) {
    return t_kw == PTR || t_kw == ARRAY;
}

bool is_pointer(Node *node) {
    return (node->kind == ND_LVAR || node->kind == ND_GVAR) &&
           is_pointer_t_kw(node->type->t_kw);
}

Type *get_node_type(Node *node) {
    Type *type;
    switch (node->kind) {
    case ND_NUM:
        return node->type;
    case ND_FUNC:
        // TODO
        error("未対応");
    case ND_LVAR:
        return node->type;
    case ND_GVAR:
        return node->type;
    case ND_ASSIGN:
        return get_node_type(node->rhs);
    case ND_ADDR:
        return new_ptr_type(get_node_type(node->lhs));
    case ND_DEREF:
        type = get_node_type(node->lhs);
        if (is_pointer_t_kw(type->t_kw))
            return type->ptr_to;
        error("*演算子の適用が不適切です");
    }

    Type *t1 = get_node_type(node->lhs);
    Type *t2 = get_node_type(node->rhs);
    switch (node->kind) {
    case ND_ADD:
    case ND_SUB:
        if (is_pointer_t_kw(t1->t_kw) && !is_pointer_t_kw(t2->t_kw)) return t1;
        if (!is_pointer_t_kw(t1->t_kw) && is_pointer_t_kw(t2->t_kw)) return t2;
    case ND_MUL:
    case ND_DIV:
    case ND_EQ:
    case ND_NE:
    case ND_GT:
    case ND_GE:
    case ND_LT:
    case ND_LE:
        if (!is_pointer_t_kw(t1->t_kw) && !is_pointer_t_kw(t2->t_kw)) return t1;
    }
    error("型の判別ができませんでした");
}

void program(Node **nodes) {
    int i = 0;
    while (!at_eof())
        nodes[i++] = fragment();
    nodes[i] = NULL;
}

Node *fragment(void) {
    Type *type = expect_type();
    char *name;
    int len;
    expect_ident(&name, &len);

    if (consume("(")) return func(type, name, len);
    else global(type, name, len);
}

Node *global(Type *type, char *name, int len) {
    define_global_variable(&type, name, len);
    expect(";");
    return new_node_ident(ND_GVAR_DEF, type, name, len);
}

Node *func(Type *return_type, char *name, int len) {
    Node *node = new_node_ident(ND_FUNC_DEF, return_type, name, len), *cur = node;

    if (!consume(")")) {
        do {
            int offset;
            Type *type = expect_type();
            define_local_variable(&type, &offset);
            cur->lhs = new_node_lvar(type, offset);
            cur = cur->lhs;
        } while (consume(","));
        expect(")");
    }

    cur = node;
    expect("{");
    while (!consume("}")) {
        cur->next = stmt();
        if (cur->next != NULL) cur = cur->next;
    }
    node->rhs = new_node_num(lvar_offset());     // 引数とローカル変数の個数を保持
    clear_lvar();
    return node;
}

Node *stmt(void) {
    Node *node;
    Type *type;

    if (consume("if")) {
        expect("(");
        Node *e = expr();
        expect(")");
        Node *s1, *s2 = NULL;
        s1 = stmt();
        if (consume("else")) s2 = stmt();
        node = new_node(ND_IF, s1, s2);
        node = new_node(ND_IF, e, node);
    }
    else if (consume("while")) {
        expect("(");
        Node *e = expr();
        expect(")");
        node = new_node(ND_WHILE, e, stmt());
    }
    else if (consume("for")) {
        expect("(");
        Node *e1 = NULL, *e2 = NULL, *e3 = NULL;
        if (!consume(";")) {
            e1 = expr();
            expect(";");
        }
        if (!consume(";")) {
            e2 = expr();
            expect(";");
        }
        if (!consume(")")) {
            e3 = expr();
            expect(")");
        }
        node = new_node(ND_FOR, e1, e2);
        node = new_node(ND_FOR, node, e3);
        node = new_node(ND_FOR, node, stmt());
    }
    else if (consume("{")) {
        if (consume("}")) return NULL;
        node = new_node(ND_BLOCK, stmt(), NULL);

        Node *cur = node->lhs;
        while (!consume("}")) {
            cur->next = stmt();
            if (cur->next != NULL) cur = cur->next;
        }
    }
    else if (consume("return")) {
        node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
    }
    else if (consume_type(&type)) {
        define_local_variable(&type, NULL);
        expect(";");
        node = NULL;
    }
    else {
        node = expr();
        expect(";");
    }
    return node;
}

Node *expr(void) {
    Node *node = equality();
    while (consume("=")) {
        node = new_node(ND_ASSIGN, node, equality());
    }
    return node;
}

Node *equality(void) {
    Node *node = relational();

    while (true) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational(void) {
    Node *node = add();

    while (true) {
        if (consume(">="))
            node = new_node(ND_GE, node, add());
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">"))
            node = new_node(ND_GT, node, add());
        else if (consume("<"))
            node = new_node(ND_LT, node, add());
        else
            return node;
    }
}

Node *add(void) {
    Node *node = mul();

    while (true) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul(void) {
    Node *node = unary();

    while (true) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(void) {
    if (consume("sizeof")) return new_node_num(get_type_size(get_node_type(unary())));
    else if (consume("*")) return new_node(ND_DEREF, unary(), NULL);
    else if (consume("&")) return new_node(ND_ADDR, unary(), NULL);
    else if (consume("-")) return new_node(ND_SUB, new_node_num(0), factor());
    consume("+");
    return factor();
}

Node *factor(void) {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Node *node;
    Type *type;
    int len, offset;
    char *name;
    if (consume_func_call(&name, &len)) {
        node = new_node_ident(ND_FUNC, NULL, name, len);
        expect("(");
        if (consume(")")) return node;

        node->lhs = expr();
        Node *cur = node->lhs;
        while (consume(",")) {
            cur->next = expr();
            cur = cur->next;
        }
        expect(")");
    }
    else if (consume_lvar(&type, &offset)) node = new_node_lvar(type, offset);
    else if (consume_gvar(&type, &name, &len)) node = new_node_ident(ND_GVAR, type, name, len);
    else node = new_node_num(expect_number());

    if (consume("[")) {
        Node *n = expr();
        expect("]");
        // x[y] == y[x] == *(x + y)
        node = new_node(ND_DEREF, new_node(ND_ADD, node, n), NULL);
    }
    return node;
}