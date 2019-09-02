int x1;
int y1;
char a;
char b;

int t1() { return 0; }

int t2() { return 42; }

int t3() { return 5+20-4; }

int t4() { return  12 + 34 - 5 ; }

int t5() { return 5+6*7; }

int t6() { return 5*(9-6); }

int t7() { return (3+5)/2; }

int t8() { return -10 + 20; }

int t9() { return 10 + +30 - -40; }

int t10() { return 1 != 2; }

int t11() { return 3 == 3; }

int t12() { return 1 < 2; }

int t13() { return 1 < 1; }

int t14() { return 2 <= 2; }

int t15() { return 3 >= 3; }

int t16() { return 4 > 4; }

int t17() { return 5 > 4; }

int t18() { return (((1 + 3) == 4) * 2 != 3) == 1; }

int t19() {
    int a;
    a = 3;
    return a;
}

int t20() {
    int a;
    int b;
    a = 3;
    b = 5 * 6 - 8;
    return a + b / 2;
}

int t21() {
    int foo;
    foo = 1;
    int bar;
    bar = 2 + 3;
    return foo + bar;
}

int t22() {
    int return10;
    return10 = 3;
    return return10;
}

int t23() {
    int ret;
    ret = 2;
    if (100) ret = 1;
    return ret;
}

int t24() {
    int ret;
    ret = 100;
    if (0) ret = 10; else ret = 2;
    return ret;
}

int t25() {
    int i;
    i = 1;
    while (i < 10) i = i + 1;
    return i;
}

int t26() {
    int i;
    int ret;
    i = 100;
    ret = 2;
    for (i = 0; i < 4; i = i + 1) ret = ret + ret;
    return ret;
}

int t27() {
    int ans;
    int i;
    int j;
    ans = 0;
    for (i = 0; i <= 10; i = i + 1) if (i < 5) ans = ans + i;
    j = 0;
    while (j < 10) j = j + 1;
    return ans;
}

int t28() {
    return foo1();
}
int foo1() {
    return 10;
}

int t29() {
    return foo2(3, 4);
}
int foo2(int x, int y) {
    return x * y;
}

int t30() {
    return fib(7);
}
int fib(int n) {
    if (n == 0) return 1;
    if (n == 1) return 1;
    return fib(n - 1) + fib(n - 2);
}

int t31() {
    return foo3(1, 2, 3, 4, 5, 6);
}
int foo3(int n1, int n2, int n3, int n4, int n5, int n6) {
    return n1 + n2 + n3 + n4 + n5 + n6;
}

int t32() {
    int x;
    int *y;
    x = 3;
    y = &x;
    return *y;
}

int t33() {
    int x;
    int y;
    int *z;
    x = 3;
    y = 5;
    z = &y + 1;
    return *z;
}

int t34() {
    int x;
    int *y;
    y = &x;
    *y = 3;
    return x;
}

int t35() {
    int x;
    int *y;
    int **z;
    y = &x;
    z = &y;
    **z = 3;
    return x;
}

int t36() {
    int p[4];
    *(p) = 1;
    *(p + 1) = 2;
    *(p + 2) = 4;
    *(p + 3) = 8;
    int *q;
    q = p + 2;
    return *q + *(p + 3);
}

int t37() {
    int a[2];
    *a = 1;
    *(a + 1) = 2;
    int *p;
    p = a;
    return *p + *(p + 1);
}

int t38() {
    int a[3];
    a[0] = 1;
    a[1] = 10;
    a[2] = 100;
    return *&a[1];
}

int t39() { int a[4]; a[2] = 10; return a[2]; }

int t40() { int a[4]; a[1] = 100; return 1[a]; }

int t41() { int x; return sizeof(x); }

int t42() { int *x; return sizeof(x); }

int t43() { int *x; return sizeof(x + 10); }

int t44() { int *x; return sizeof(*x); }

int t45() { int x; return sizeof(*&x); }

int t46() { int x; return sizeof(&*&x); }

int t47() { return sizeof(1); }

int t48() { return sizeof(sizeof 1); }

int t49() { int a[4]; return sizeof(a); }

int t50() { int *a[4]; return sizeof(a); }

int t51() { char x[7]; return sizeof(x); }

int t52() {
    x1 = 100;
    return x1;
}

int t53() {
    x1 = 20;
    int x1;
    x1 = 100;
    return x1 + global();
}
int global() {
    return x1;
}

int t54() {
    x1 = 10;
    y1 = 7;
    int z;
    z = 5;
    return x1 + y1 + z;
}

int t55() {
    char x[3];
    x[0] = -1;
    x[1] = 2;
    int y;
    y = 4;
    return x[0] + y;
}

int t56() { char *x; x = "hello world!"; return x[0]; }

int t57() { char *x; x = "hello world!"; return x[6]; }

int t58() { char *x; x = "hello world!"; return x[11]; }

int t59() {
    a = -1;
    b = 6;
    return a + b;
}

int t60() {
    int x;  // x
    // comment
    x = 10;
    return x;
}

int t61() {
    int x;  /* x
    x = 20;
    comment!
    */
    x = 15;
    return x;
}

int t62() {
    int x[2][2];
    x[0][0] = 1;
    x[0][1] = 2;
    x[1][0] = 3;
    x[1][1] = 4;
    return x[0][1] + x[1][0];
}

int t63() {
    char x[2][3][5];
    x[0][2][0] = 23;
    x[0][2][1] = -1;
    x[0][2][2] = 2;
    x[1][0][4] = 3;
    return x[0][2][1] + x[1][0][4];
}

int t64() {
    int x = 17;
    return x;
}

int t65() {
    char a = 1;
    char b = -7;
    char c = 29;
    return a + b + c;
}

int t66() {
    int x[3] = {1, 5, 3};
    return x[0] + x[1] + x[2];
}

int t67() {
    char x[3] = {1, -5, 3};
    return x[0] + x[1] + x[2];
}

int t68() {
    int x[3] = {1, 5, 3};
    return x[0] + x[1];
}

int t69() {
    int x[3] = {1, 5};
    return x[0] + x[2];
}

int main() {
    println("*** start arithmetic test ***");
    {
        test(t1(), 0 );
        test(t2(), 42);
        test(t3(), 21);
        test(t4(), 41);
        test(t5(), 47);
        test(t6(), 15);
        test(t7(), 4 );
        test(t8(), 10);
        test(t9(), 80);
        test(t10(), 1);
        test(t11(), 1);
        test(t12(), 1);
        test(t13(), 0);
        test(t14(), 1);
        test(t15(), 1);
        test(t16(), 0);
        test(t17(), 1);
        test(t18(), 1);
        reset_test_no();
        println("");
    }

    println("*** start variable test ***");
    {
        test(t19(), 3);
        test(t20(), 14);
        test(t21(), 6);
        test(t22(), 3);
        reset_test_no();
        println("");
    }

    println("*** start control test ***");
    {
        test(t23(), 1);
        test(t24(), 2);
        test(t25(), 10);
        test(t26(), 32);
        test(t27(), 10);
        reset_test_no();
        println("");
    }

    println("*** start function call test ***");
    {
        test(t28(), 10);
        test(t29(), 12);
        test(t30(), 21);
        test(t31(), 21);
        reset_test_no();
        println("");
    }

    println("*** start pointer and array test ***");
    {
        test(t32(), 3);
        test(t33(), 3);
        test(t34(), 3);
        test(t35(), 3);
        test(t36(), 12);
        test(t37(), 3);
        test(t38(), 10);
        test(t39(), 10);
        test(t40(), 100);
        reset_test_no();
        println("");
    }

    println("*** start sizeof test ***");
    {
        test(t41(), 4);
        test(t42(), 8);
        test(t43(), 8);
        test(t44(), 4);
        test(t45(), 4);
        test(t46(), 8);
        test(t47(), 4);
        test(t48(), 4);
        test(t49(), 16);
        test(t50(), 32);
        test(t51(), 7);
        reset_test_no();
        println("");
    }

    println("*** start global variable test ***");
    {
        test(t52(), 100);
        test(t53(), 120);
        test(t54(), 22);
        reset_test_no();
        println("");
    }

    println("*** start char test ***");
    {
        test(t55(), 3);
        test(t56(), 104);
        test(t57(), 119);
        test(t58(), 33);
        test(t59(), 5);
        reset_test_no();
        println("");
    }

    println("*** start comment test ***");
    {
        test(t60(), 10);
        test(t61(), 15);
        reset_test_no();
        println("");
    }

    println("*** start multi dimensional array test ***");
    {
        test(t62(), 5);
        test(t63(), 2);
        reset_test_no();
        println("");
    }
    println("*** start initialize test ***");
    {
        test(t64(), 17);
        test(t65(), 23);
        test(t66(), 9);
        test(t67(), -1);
        test(t68(), 6);
        test(t69(), 1);
        reset_test_no();
        println("");
    }
    return 0;
}