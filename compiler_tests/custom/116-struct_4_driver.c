struct x {
    int a;
    int b;
    int c;
    int d;
};

int f(struct x a);

int main()
{
    struct x a;

    a.a = 1;
    a.b = 2;
    a.c = 3;
    a.d = 4;
    if (!(f(a) == 10)) return 1;

    a.a = 5;
    a.b = 6;
    a.c = 7;
    a.d = 8;
    if (!(f(a) == 26)) return 2;

    return 0;
}
