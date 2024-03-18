struct x {
    int a;
    char b;
    float c;
};

int f() {
    struct x b;
    return sizeof(b);
}
