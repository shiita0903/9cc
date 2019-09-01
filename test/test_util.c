#include <stdio.h>

int no = 1;

void print(char *str) {
    printf("%s", str);
}

void println(char *str) {
    printf("%s\n", str);
}

void test(int actual, int expected) {
    if (actual == expected)
        printf("  No.%3d test is success\n", no++);
    else
        printf("  No.%3d test is failure: %d expected, but got %d\n", no++, expected, actual);
}

void reset_test_no() {
    no = 1;
}