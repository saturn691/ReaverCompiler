int f(int x);

int main()
{
    if (!(f(0) == 0)) return 1;
    if (!(f(1) == 1)) return 1;
    if (!(f(2) == 2)) return 1;
    if (!(f(3) == 3)) return 1;

    return 0;
}
