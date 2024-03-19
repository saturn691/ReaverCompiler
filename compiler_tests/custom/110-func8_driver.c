int f(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7);

int main(void)
{
    if (!f(1, 2, 3, 4, 5, 6, 7, 8) == 36) return 1;
    if (!f(2, 3, 4, 5, 6, 7, 8, 9) == 44) return 2;

    return 0;
}
