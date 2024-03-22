int f(int *arr, int n);

int main()
{
    int x[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    if (f(x, 0) != 0) return 1;
    if (f(x, 1) != 1) return 2;
    if (f(x, 2) != 2) return 3;
    if (f(x, 3) != 3) return 4;
    if (f(x, 4) != 4) return 5;
    if (f(x, 5) != 5) return 6;
    if (f(x, 6) != 6) return 7;
    if (f(x, 7) != 7) return 8;
    if (f(x, 8) != 8) return 9;
    if (f(x, 9) != 9) return 10;

    return 0;
}
