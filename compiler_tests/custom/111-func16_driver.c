int f(
    int a0, int a1, int a2, int a3,
    int a4, int a5, int a6, int a7,
    int a8, int a9, int a10, int a11,
    int a12, int a13, int a14, int a15
);

int main(void)
{
    if (!(f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16) == 136))
    {
        return 1;
    }

    return 0;
}
