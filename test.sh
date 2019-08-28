#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
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
    int y;
    x = 3;
    y = &x;
    return *y;
}'
try 3   'int main() {
    int x;
    int y;
    int z;
    x = 3;
    y = 5;
    z = &y + 8;
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

echo OK