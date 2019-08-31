#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -no-pie -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try 0   'int main() { return 0; }'
try 42  'int main() { return 42; }'
try 21  'int main() { return 5+20-4; }'
try 41  'int main() { return  12 + 34 - 5 ; }'
try 47  'int main() { return 5+6*7; }'
try 15  'int main() { return 5*(9-6); }'
try 4   'int main() { return (3+5)/2; }'
try 10  'int main() { return -10 + 20; }'
try 80  'int main() { return 10 + +30 - -40; }'
try 1   'int main() { return 1 != 2; }'
try 1   'int main() { return 3 == 3; }'
try 1   'int main() { return 1 < 2; }'
try 0   'int main() { return 1 < 1; }'
try 1   'int main() { return 2 <= 2; }'
try 1   'int main() { return 3 >= 3; }'
try 0   'int main() { return 4 > 4; }'
try 1   'int main() { return 5 > 4; }'
try 1   'int main() { return (((1 + 3) == 4) * 2 != 3) == 1; }'
try 3   'int main() {
    int a;
    a = 3;
    return a;
}'
try 14  'int main() {
    int a;
    int b;
    a = 3;
    b = 5 * 6 - 8;
    return a + b / 2;
}'
try 6   'int main() {
    int foo;
    foo = 1;
    int bar;
    bar = 2 + 3;
    return foo + bar;
}'
try 3   'int main() {
    int return10;
    return10 = 3;
    return return10;
}'
try 1   'int main() {
    int ret;
    ret = 2;
    if (100) ret = 1;
    return ret;
}'
try 2   'int main() {
    int ret;
    ret = 100;
    if (0) ret = 10; else ret = 2;
    return ret;
}'
try 10  'int main() {
    int i;
    i = 1;
    while (i < 10) i = i + 1;
    return i;
}'
try 32  'int main() {
    int i;
    int ret;
    i = 100;
    ret = 2;
    for (i = 0; i < 4; i = i + 1) ret = ret + ret;
    return ret;
}'
try 10  'int main() {
    int ans;
    int i;
    int j;
    ans = 0;
    for (i = 0; i <= 10; i = i + 1) if (i < 5) ans = ans + i;
    j = 0;
    while (j < 10) j = j + 1;
    return ans;
}'
try 5   'int main() { int i; int j; i = 0; j = 3; i = j + 2; return i; }'
try 10  'int main() {
    return foo();
}
int foo() {
    return 10;
}'
try 12  'int main() {
    return foo(3, 4);
}
int foo(int x, int y) {
    return x * y;
}'
try 21  'int main() {
    return fib(7);
}
int fib(int n) {
    if (n == 0) return 1;
    if (n == 1) return 1;
    return fib(n - 1) + fib(n - 2);
}'
try 21   'int main() {
    return add(1, 2, 3, 4, 5, 6);
}
int add(int n1, int n2, int n3, int n4, int n5, int n6) {
    return n1 + n2 + n3 + n4 + n5 + n6;
}'
try 3   'int main() {
    int x;
    int *y;
    x = 3;
    y = &x;
    return *y;
}'
try 3   'int main() {
    int x;
    int y;
    int *z;
    x = 3;
    y = 5;
    z = &y + 4;
    return *z;
}'
try 3   'int main() {
    int x;
    int *y;
    y = &x;
    *y = 3;
    return x;
}'
try 3   'int main() {
    int x;
    int *y;
    int **z;
    y = &x;
    z = &y;
    **z = 3;
    return x;
}'
try 12  'int main() {
    int p[4];
    *(p) = 1;
    *(p + 1) = 2;
    *(p + 2) = 4;
    *(p + 3) = 8;
    int *q;
    q = p + 2;
    return *q + *(p + 3);
}'
try 4   'int main() { int x; return sizeof(x); }'
try 8   'int main() { int *x; return sizeof(x); }'
try 8   'int main() { int *x; return sizeof(x + 10); }'
try 4   'int main() { int *x; return sizeof(*x); }'
try 4   'int main() { int x; return sizeof(*&x); }'
try 8   'int main() { int x; return sizeof(&*&x); }'
try 4   'int main() { return sizeof(1); }'
try 4   'int main() { return sizeof(sizeof 1); }'
try 3   'int main() {
    int a[2];
    *a = 1;
    *(a + 1) = 2;
    int *p;
    p = a;
    return *p + *(p + 1);
}'
try 10  'int main() {
    int a[3];
    a[0] = 1;
    a[1] = 10;
    a[2] = 100;
    return *&a[1];
}'
try 16  'int main() { int a[4]; return sizeof(a); }'
try 32  'int main() { int *a[4]; return sizeof(a); }'
try 10  'int main() { int a[4]; a[2] = 10; return a[2]; }'
try 100 'int main() { int a[4]; a[1] = 100; return 1[a]; }'
try 100 'int x;
int main() {
    x = 100;
    return x;
}'
try 120 'int x;
int main() {
    x = 20;
    int x;
    x = 100;
    return x + 20;
}
int global() {
    return 20;
}'
try 22  'int x;
int y;
int main() {
    x = 10;
    y = 7;
    int z;
    z = 5;
    return x + y + z;
}'
try 3   'int main() {
    char x[3];
    x[0] = -1;
    x[1] = 2;
    int y;
    y = 4;
    return x[0] + y;
}'

echo OK