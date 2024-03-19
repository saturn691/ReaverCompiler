struct x {
    int a;
    int b;
};

int f(struct x a);

int main()
{
    struct x a;

    a.a = 1;
    a.b = 2;
    if (!(f(a) == 3)) return 1;

    a.a = 3;
    a.b = 4;
    if (!(f(a) == 7)) return 2;

    return 0;
}
