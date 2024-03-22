int f(int *arr, int n);

int main()
{
    int x[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    int* top = x + 9;

    if (f(top, 0) != 0) return 1;
    if (f(top, 1) != 1) return 2;
    if (f(top, 2) != 2) return 3;
    if (f(top, 3) != 3) return 4;
    if (f(top, 4) != 4) return 5;
    if (f(top, 5) != 5) return 6;
    if (f(top, 6) != 6) return 7;
    if (f(top, 7) != 7) return 8;
    if (f(top, 8) != 8) return 9;
    if (f(top, 9) != 9) return 10;

    return 0;
}
