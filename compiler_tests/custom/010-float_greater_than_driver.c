int f(float x, float y);

int main()
{
    if (f(1.0, 2.0) != 0) return 1;
    if (f(2.0, 1.0) != 1) return 1;
    if (f(1.0, 1.0) != 0) return 1;
    return 0;
}
