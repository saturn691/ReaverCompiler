struct x {
    int a;
    float b;
};

float g(struct x s);

int main()
{
    struct x s;
    s.a = 5;
    s.b = 6.0;
    return (g(s) != 11.0);
}
