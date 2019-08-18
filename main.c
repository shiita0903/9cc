#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) error("引数の個数が正しくありません");

    tokenize(argv[1]);
    Node *nodes[100];    // とりあえず100
    program(nodes);
    code_gen(nodes);
    return 0;
}