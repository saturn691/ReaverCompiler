struct myStruct {
    int a;
    char b;
    float c;
};

int f(int a, char b, float c)
{
    struct myStruct s;

    s.a = a;
    s.b = b;
    s.c = c;

    return s.a;
}
