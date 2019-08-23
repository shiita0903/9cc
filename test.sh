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

try 0 'return 0;'
try 42 'return 42;'
try 21 'return 5+20-4;'
try 41 'return  12 + 34 - 5 ;'
try 47 'return 5+6*7;'
try 15 'return 5*(9-6);'
try 4 'return (3+5)/2;'
try 10 'return -10 + 20;'
try 80 'return 10 + +30 - -40;'
try 1 'return 1 != 2;'
try 1 'return 3 == 3;'
try 1 'return 1 < 2;'
try 0 'return 1 < 1;'
try 1 'return 2 <= 2;'
try 1 'return 3 >= 3;'
try 0 'return 4 > 4;'
try 1 'return 5 > 4;'
try 1 'return (((1 + 3) == 4) * 2 != 3) == 1;'
try 14 '
a = 3;
b = 5 * 6 - 8;
return a + b / 2;'
try 6 '
foo = 1;
bar = 2 + 3;
return foo + bar;'
try 1 'return 1;'
try 10 '
return 10;
3;'
try 3 '
return10 = 3;
return return10;'
try 1 '
ret = 2;
if (100) ret = 1;
return ret;'
try 2 '
ret = 100;
if (0) ret = 10; else ret = 2;
return ret;'
try 10 '
i = 1;
while (i < 10) i = i + 1;
return i;'
try 32 '
i = 100;
ret = 2;
for (i = 0; i < 4; i = i + 1) ret = ret + ret;
return ret;'
try 10 '
ans = 0;
for (i = 0; i <= 10; i = i + 1) if (i < 5) ans = ans + i;
j = 0;
while (j < 10) j = j + 1;
return ans;'
try 5 '{ i = 0; j = 3; i = j + 2; return i; }'

echo OK