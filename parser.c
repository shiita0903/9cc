#include "9cc.h"

Node *stmt(void);
Node *expr(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *factor(void);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Node *cur) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    node->next = NULL;
    if (cur != NULL) cur->next = node;
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
    Node *node;

    if (consume("if")) {
        expect("(");
        Node *e = expr();
        expect(")");
        Node *s1, *s2 = NULL;
        s1 = stmt();
        if (consume("else")) s2 = stmt();
        node = new_node(ND_IF, s1, s2, NULL);
        node = new_node(ND_IF, e, node, NULL);
    }
    else if (consume("while")) {
        expect("(");
        Node *e = expr();
        expect(")");
        node = new_node(ND_WHILE, e, stmt(), NULL);
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
        node = new_node(ND_FOR, e1, e2, NULL);
        node = new_node(ND_FOR, node, e3, NULL);
        node = new_node(ND_FOR, node, stmt(), NULL);
    }
    else if (consume("{")) {
        Node n, *cur = &n;
        while (!consume("}")) cur = new_node(ND_BLOCK, stmt(), NULL, cur);
        node = n.next;
    }
    else if (consume("return")) {
        node = new_node(ND_RETURN, expr(), NULL, NULL);
        expect(";");
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
        node = new_node(ND_ASSIGN, node, equality(), NULL);
    }
    return node;
}

Node *equality(void) {
    Node *node = relational();

    while (true) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational(), NULL);
        else if (consume("!="))
            node = new_node(ND_NE, node, relational(), NULL);
        else
            return node;
    }
}

Node *relational(void) {
    Node *node = add();

    while (true) {
        if (consume(">="))
            node = new_node(ND_GE, node, add(), NULL);
        else if (consume("<="))
            node = new_node(ND_LE, node, add(), NULL);
        else if (consume(">"))
            node = new_node(ND_GT, node, add(), NULL);
        else if (consume("<"))
            node = new_node(ND_LT, node, add(), NULL);
        else
            return node;
    }
}

Node *add(void) {
    Node *node = mul();

    while (true) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul(), NULL);
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul(), NULL);
        else
            return node;
    }
}

Node *mul(void) {
    Node *node = unary();

    while (true) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary(), NULL);
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary(), NULL);
        else
            return node;
    }
}

Node *unary(void) {
    if (consume("-")) return new_node(ND_SUB, new_node_num(0), factor(), NULL);
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