struct x {
    int a;
    int b;
    int c;
    int d;
};

int f() {
    struct x b;
    return sizeof(b);
}
