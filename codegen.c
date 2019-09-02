#include "9cc.h"

void gen(Node *node);

long long if_sn = 0, while_sn = 0, for_sn = 0;

void gen_left_value(Node *node) {
    switch (node->kind) {
    case ND_LVAR:
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", node->offset);
        printf("  push rax\n");
        break;
    case ND_GVAR:
        printf("  lea rax, %.*s\n", node->len, node->name);
        printf("  push rax\n");
        break;
    case ND_DEREF:
        gen(node->lhs);
        break;
    default:
        error("代入の左辺値が変数ではありません");
    }
}

void gen_pointer_adjust(Node *node, char *r_name) {
    Type *type = get_node_type(node);

    switch (type->t_kw) {
    case PTR:
        switch(type->ptr_to->t_kw) {
        case INT:
            printf("  shl %s, 2\n", r_name);
            break;
        case CHAR:
            break;
        case PTR:
            printf("  shl %s, 3\n", r_name);
            break;
        case ARRAY:
            error("配列のポインタは未実装です");
            break;
        }
        break;
    case ARRAY:
        printf("  imul %s, %d\n", r_name, get_type_size(type->ptr_to));
        break;
    }
}

void get_addr_value(Node *node, bool is_deref) {
    TypeKeyword t_kw;
    if (is_deref) t_kw = get_node_type(node)->ptr_to->t_kw;
    else t_kw = get_node_type(node)->t_kw;

    printf("  pop rax\n");
    switch (t_kw) {
    case INT:
        printf("  mov eax, [rax]\n");
        break;
    case CHAR:
        printf("  movsx rax, BYTE PTR [rax]\n");
        break;
    case PTR:
        printf("  mov rax, [rax]\n");
        break;
    case ARRAY:
        if (is_deref) printf("  mov rax, [rax]\n");
        break;
    }
    printf("  push rax\n");
}

void assign_value(Node *node) {
    printf("  pop rdi\n");
    printf("  pop rax\n");
    switch (get_node_type(node)->t_kw) {
    case INT:
        printf("  mov [rax], edi\n");
        printf("  mov eax, edi\n");
        break;
    case CHAR:
        printf("  mov [rax], dil\n");
        printf("  movsx eax, dil\n");
        break;
    case PTR:
    case ARRAY:
        printf("  mov [rax], rdi\n");
        printf("  mov rax, rdi\n");
        break;
    }
    printf("  push rax\n");
}

int get_size_i(Node *node) {
    switch (node->type->t_kw) {
    case INT:
        return 1;
    case CHAR:
        return 2;
    case PTR:
    case ARRAY:
        return 0;
    }
}

void gen(Node *node) {
    if (node == NULL) return;

    Node *cur;
    Type *type;
    int arg_num;
    char *r_name[3][6] = {
        { "rdi", "rsi", "rdx", "rcx", "r8", "r9" },
        { "edi", "esi", "edx", "ecx", "r8d", "r9d" },
        { "dil", "dil", "dl", "cl", "r8b", "r9b" }
    };

    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_STR:
        printf("  lea rax, .Lstr%d\n", node->val);
        printf("  push rax\n");
        return;
    case ND_FUNC:
        arg_num = 0;
        char *name = node->name;
        int len = node->len;

        cur = node->lhs;
        while (cur != NULL) {
            gen(cur);
            cur = cur->next;
            arg_num++;
        }

        for (int i = arg_num - 1; i >= 0 ; i--)
            printf("  pop %s\n", r_name[0][i]);

        // TODO: RSPを16の倍数にする処理が必要らしい
        printf("  call %.*s\n", len, name);
        printf("  push rax\n");
        return;
    case ND_LVAR:
    case ND_GVAR:
        gen_left_value(node);
        get_addr_value(node, false);
        return;
    case ND_GVAR_DEF:
        printf("%.*s:\n", node->len, node->name);
        printf("  .zero %d\n", get_type_size(node->type));
        return;
    case ND_ASSIGN:
        gen_left_value(node->lhs);
        gen(node->rhs);
        assign_value(node->lhs);
        return;
    case ND_INIT:
        type = node->lhs->type;
        if (type->t_kw == ARRAY) {
            int array_count = get_type_size(type) / get_type_size(get_array_base_type(type));
            int node_cnt = 0;
            for (Node *n = node->rhs; n != NULL; n = n->next)
                node_cnt++;

            // ゼロ埋め
            for (int i = 0; i < array_count - node_cnt; i++)
                printf("  push 0\n");
            // 初期化要素の評価
            for (Node *n = node->rhs; n != NULL; n = n->next)
                gen(n);

            gen_left_value(node->lhs);
            printf("  pop rax\n");
            TypeKeyword t_kw = get_array_base_type(type)->t_kw;
            for (int i = 0; i < array_count; i++) {
                switch (t_kw) {
                case INT:
                    printf("  pop rdi\n");
                    printf("  mov [rax], edi\n");
                    printf("  add rax, 4\n");
                    break;
                case CHAR:
                    printf("  pop rdi\n");
                    printf("  mov [rax], dil\n");
                    printf("  add rax, 1\n");
                    break;
                case PTR:
                    printf("  pop rdi\n");
                    printf("  mov [rax], rdi\n");
                    printf("  add rax, 8\n");
                    break;
                }
            }
        }
        else error("初期化に失敗しました");
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
        cur = node->lhs;
        while (cur != NULL) {
            gen(cur);
            cur = cur->next;
        }
        return;
    case ND_RETURN:
        gen(node->lhs);

        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_FUNC_DEF:
        arg_num = 0;
        printf("%.*s:\n", node->len, node->name);
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, %d\n", node->rhs->val);

        cur = node->lhs;
        while (cur != NULL) {
            int size_i = get_size_i(cur);

            printf("  mov rax, rbp\n");
            printf("  sub rax, %d\n", cur->offset);
            printf("  mov [rax], %s\n", r_name[size_i][arg_num++]);
            cur = cur->lhs;
        }
        node = node->next;
        while (node != NULL) {
            gen(node);
            node = node->next;
        }
        return;
    case ND_ADDR:
        gen_left_value(node->lhs);
        return;
    case ND_DEREF:
        gen(node->lhs);
        get_addr_value(node->lhs, true);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        gen_pointer_adjust(node->lhs, "rdi");
        gen_pointer_adjust(node->rhs, "rax");
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        gen_pointer_adjust(node->lhs, "rdi");
        gen_pointer_adjust(node->rhs, "rax");
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
    Node **n = nodes;
    printf(".intel_syntax noprefix\n");

    int *lens = calloc(MAX_STR_COUNT, sizeof(int));
    char **names = calloc(MAX_NODE_COUNT, sizeof(char *));
    int cnt = get_strs(&names, &lens);

    printf(".data\n");
    for (int i = 0; i < cnt; i++) {
        printf(".Lstr%d:\n", i);
        printf("  .string \"%.*s\"\n", lens[cnt - i - 1], names[cnt - i - 1]);
    }

    for (; *n != NULL && (*n)->kind == ND_GVAR_DEF; n++)
        gen(*n);

    printf(".text\n");
    printf(".global main\n");
    for (; *n != NULL; n++)
        gen(*n);
}