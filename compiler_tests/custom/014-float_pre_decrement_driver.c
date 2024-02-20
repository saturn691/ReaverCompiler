float f(float x);

int main()
{
    if (f(3.0) != 2.0) return 1;
    if (f(3.5) != 2.5) return 1;
    return 0;
}
