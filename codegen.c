#include "9cc.h"

long long if_sn = 0, while_sn = 0, for_sn = 0;

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {
    if (node == NULL) return;

    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);

        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_IF:
        gen(node->lhs);

        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (node->rhs->rhs == NULL) {
            printf("  je .Lend_if_%lld\n", if_sn);
            gen(node->rhs->lhs);
            printf(".Lend_if_%lld:\n", if_sn);
        }
        else {
            printf("  je .Lelse_if_%lld\n", if_sn);
            gen(node->rhs->lhs);
            printf("jmp .Lend_if_%lld\n", if_sn);
            printf(".Lelse_if_%lld:\n", if_sn);
            gen(node->rhs->rhs);
            printf(".Lend_if_%lld:\n", if_sn);
        }
        if_sn++;
        return;
    case ND_WHILE:
        printf(".Lbegin_while_%lld:\n", while_sn);
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend_while_%lld\n", while_sn);
        gen(node->rhs);
        printf("  jmp .Lbegin_while_%lld\n", while_sn);
        printf(".Lend_while_%lld:\n", while_sn);
        while_sn++;
        return;
    case ND_FOR:
        gen(node->lhs->lhs->lhs);
        printf(".Lbegin_for_%lld:\n", for_sn);
        gen(node->lhs->lhs->rhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend_for_%lld\n", for_sn);
        gen(node->rhs);
        gen(node->lhs->rhs);
        printf("  jmp .Lbegin_for_%lld\n", for_sn);
        printf(".Lend_for_%lld:\n", for_sn);
        for_sn++;
        return;
    case ND_BLOCK:
        while (node != NULL) {
            gen(node->lhs);
            printf("  pop rax\n");
            node = node->next;
        }
        return;
    case ND_RETURN:
        gen(node->lhs);

        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_GT:
        printf("  cmp rdi, rax\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_GE:
        printf("  cmp rdi, rax\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

void code_gen(Node **nodes) {
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 変数の領域確保。一時的な処理
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", 8 * lvar_count());

    while (*nodes != NULL) {
        gen(*nodes);
        printf("  pop rax\n");  // スタックのゴミ削除 && 戻り値のセット
        nodes++;
    }
}