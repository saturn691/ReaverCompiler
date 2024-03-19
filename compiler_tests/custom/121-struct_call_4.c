struct x {
    int a;
    float b;
    int c;
    float d;
};

float f(struct x s)
{
    return s.a + s.b + s.c + s.d;
}

float g(struct x s)
{
    return f(s);
}
