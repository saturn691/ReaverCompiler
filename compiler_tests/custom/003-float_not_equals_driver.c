int f(float x, float y);

int main()
{
    if (f(0.0, 0.0) == 1) return 1;
    if (f(0.0, 1.0) == 0) return 1;
    if (f(1.0, 0.0) == 0) return 1;
    if (f(1.0, 1.0) == 1) return 1;
    if (f(-1.0, -1.0) == 1) return 1;
    if (f(0.5, 0.5) == 1) return 1;
    if (f(0.5, 0.51) == 0) return 1;
    return 0;
}
