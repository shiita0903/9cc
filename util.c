#include "9cc.h"
#include <errno.h>

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

char *read_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp)
    error("cannot open %s: %s", path, strerror(errno));

    // ファイルの長さを調べる
    if (fseek(fp, 0, SEEK_END) == -1)
    error("%s: fseek: %s", path, strerror(errno));
    size_t size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s: fseek: %s", path, strerror(errno));

    // ファイル内容を読み込む
    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, fp);

    // ファイルが必ず"\n\0"で終わっているようにする
    if (size == 0 || buf[size - 1] != '\n')
       buf[size++] = '\n';
    buf[size] = '\0';

    fclose(fp);
    return buf;
}

void print_t_kw(Type *type) {
    switch (type->t_kw)
    {
    case INT:
        printf("INT ");
        break;
    case CHAR:
        printf("CHAR ");
        break;
    case PTR:
        printf("PTR ");
        break;
    case ARRAY:
        printf("ARRAY(%d) ", type->array_size);
        break;
    }
}

void print_type(Type *type) {
    for (Type *t = type; t != NULL; t = t->ptr_to)
        print_t_kw(t);
    printf("\n");
}

void print_node_kind(NodeKind kind) {
    switch (kind) {
    case ND_ADD:
        printf("ND_ADD\n");
        break;
    case ND_SUB:
        printf("ND_SUB\n");
        break;
    case ND_MUL:
        printf("ND_MUL\n");
        break;
    case ND_DIV:
        printf("ND_DIV\n");
        break;
    case ND_NUM:
        printf("ND_NUM\n");
        break;
    case ND_STR:
        printf("ND_STR\n");
        break;
    case ND_EQ:
        printf("ND_EQ\n");
        break;
    case ND_NE:
        printf("ND_NE\n");
        break;
    case ND_GT:
        printf("ND_GT\n");
        break;
    case ND_GE:
        printf("ND_GE\n");
        break;
    case ND_LT:
        printf("ND_LT\n");
        break;
    case ND_LE:
        printf("ND_LE\n");
        break;
    case ND_ASSIGN:
        printf("ND_ASSIGN\n");
        break;
    case ND_LVAR:
        printf("ND_LVAR\n");
        break;
    case ND_GVAR:
        printf("ND_GVAR\n");
        break;
    case ND_GVAR_DEF:
        printf("ND_GVAR_DEF\n");
        break;
    case ND_IF:
        printf("ND_IF\n");
        break;
    case ND_WHILE:
        printf("ND_WHILE\n");
        break;
    case ND_FOR:
        printf("ND_FOR\n");
        break;
    case ND_RETURN:
        printf("ND_RETURN\n");
        break;
    case ND_BLOCK:
        printf("ND_BLOCK\n");
        break;
    case ND_FUNC:
        printf("ND_FUNC\n");
        break;
    case ND_FUNC_DEF:
        printf("ND_FUNC_DEF\n");
        break;
    case ND_ADDR:
        printf("ND_ADDR\n");
        break;
    case ND_DEREF:
        printf("ND_DEREF\n");
    }
}

void print_node(Node *node) {
    print_node_kind(node->kind);
}