#include "9cc.h"

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
    node->val = val;
    return node;
}

Node *new_node_ident(int offset) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = offset;
    return node;
}

void program(Node **nodes) {
    int i = 0;
    while (!at_eof())
        nodes[i++] = stmt();
    nodes[i] = NULL;
}

Node *stmt(void) {
    Node *node = expr();
    expect(";");
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
    if (consume("-")) return new_node(ND_SUB, new_node_num(0), factor());
    consume("+");
    return factor();
}

Node *factor(void) {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    int offset;
    if (consume_ident(&offset)) return new_node_ident(offset);

    return new_node_num(expect_number());
}