float f(float x);

int main()
{
    if (f(3.0) != 3.0) return 1;
    if (f(3.5) != 3.5) return 1;
    return 0;
}
