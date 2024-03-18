struct x {
    int a;
    int b;
    int c;
    int d;
};

int f(struct x a) {
    return a.a + a.b + a.c + a.d;
}
