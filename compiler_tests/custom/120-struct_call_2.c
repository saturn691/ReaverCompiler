/**
 *  The correct way to pass this struct would be to pass it through the
 *  argument registers (a0 and fa0).
*/

struct x {
    int a;
    float b;
};

float f(struct x s)
{
    return s.a + s.b;
}

float g(struct x s)
{
    return f(s);
}
