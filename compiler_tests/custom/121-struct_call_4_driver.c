struct x {
    int a;
    float b;
    int c;
    float d;
};

float g(struct x s);

int main()
{
    struct x s;
    s.a = 5;
    s.b = 6.0;
    s.c = 7;
    s.d = 8.0;
    return (g(s) != 26.0);
}
