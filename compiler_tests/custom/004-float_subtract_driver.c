float f(float x, float y);

int main()
{
    if (f(0.0, 0.0) != 0.0) return 1;
    if (f(0.0, 1.0) != -1.0) return 1;
    if (f(1.0, 0.0) != 1.0) return 1;
    if (f(1.0, 1.0) != 0.0) return 1;
    if (f(-1.0, -1.0) != 0.0) return 1;
    if (f(0.5, 0.5) != 0.0) return 1;
    // Floating point subtraction is not exact, allow for some error
    if (f(0.5, 0.4) < 0.09999 || f(0.5, 0.4) > 0.10001) return 1;
    return 0;
}
