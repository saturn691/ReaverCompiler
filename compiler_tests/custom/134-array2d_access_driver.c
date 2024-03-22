#include <stdio.h>

int access(int arr[2][2], int i, int j);

int main()
{
    int f[2][2];
    f[0][0] = 1;
    f[0][1] = 2;
    f[1][0] = 3;
    f[1][1] = 4;

    if (access(f, 0, 0) != 1)
    {
        printf("Error: %d\n", access(f, 0, 0));
        return 1;
    }

    if (access(f, 0, 1) != 2)
    {
        printf("Error: %d\n", access(f, 0, 1));
        return 2;
    }

    if (access(f, 1, 0) != 3)
    {
        printf("Error: %d\n", access(f, 1, 0));
        return 3;
    }

    if (access(f, 1, 1) != 4)
    {
        printf("Error: %d\n", access(f, 1, 1));
        return 4;
    }

    return 0;
}
