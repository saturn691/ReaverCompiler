int f(int x);

int main()
{
    if (f(0) != 3) return 1;
    if (f(1) != 2) return 1;
    return 0;
}
