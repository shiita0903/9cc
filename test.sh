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

try 0   'main() { return 0; }'
try 42  'main() { return 42; }'
try 21  'main() { return 5+20-4; }'
try 41  'main() { return  12 + 34 - 5 ; }'
try 47  'main() { return 5+6*7; }'
try 15  'main() { return 5*(9-6); }'
try 4   'main() { return (3+5)/2; }'
try 10  'main() { return -10 + 20; }'
try 80  'main() { return 10 + +30 - -40; }'
try 1   'main() { return 1 != 2; }'
try 1   'main() { return 3 == 3; }'
try 1   'main() { return 1 < 2; }'
try 0   'main() { return 1 < 1; }'
try 1   'main() { return 2 <= 2; }'
try 1   'main() { return 3 >= 3; }'
try 0   'main() { return 4 > 4; }'
try 1   'main() { return 5 > 4; }'
try 1   'main() { return (((1 + 3) == 4) * 2 != 3) == 1; }'
try 3   'main() {
    a = 3;
    return a;
}'
try 14  'main() {
    a = 3;
    b = 5 * 6 - 8;
    return a + b / 2;
}'
try 6   'main() {
    foo = 1;
    bar = 2 + 3;
    return foo + bar;
}'
try 3   'main() {
    return10 = 3;
    return return10;
}'
try 1   'main() {
    ret = 2;
    if (100) ret = 1;
    return ret;
}'
try 2   'main() {
    ret = 100;
    if (0) ret = 10; else ret = 2;
    return ret;
}'
try 10  'main() {
    i = 1;
    while (i < 10) i = i + 1;
    return i;
}'
try 32  'main() {
    i = 100;
    ret = 2;
    for (i = 0; i < 4; i = i + 1) ret = ret + ret;
    return ret;
}'
try 10  'main() {
    ans = 0;
    for (i = 0; i <= 10; i = i + 1) if (i < 5) ans = ans + i;
    j = 0;
    while (j < 10) j = j + 1;
    return ans;
}'
try 5   'main() { i = 0; j = 3; i = j + 2; return i; }'
try 10  'main() {
    return foo();
}
foo() {
    return 10;
}'
try 12  'main() {
    return foo(3, 4);
}
foo(x, y) {
    return x * y;
}'
try 21  'main() {
    return fib(7);
}
fib(n) {
    if (n == 0) return 1;
    if (n == 1) return 1;
    return fib(n - 1) + fib(n - 2);
}'

echo OK