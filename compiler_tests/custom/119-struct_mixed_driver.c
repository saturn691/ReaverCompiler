int f(int a, char b, float c);

int main()
{
    if (f(1, 'x', 3.0) != 1) return 1;
    if (f(120, 'x', 3.0) != 120) return 1;
    return 0;
}
