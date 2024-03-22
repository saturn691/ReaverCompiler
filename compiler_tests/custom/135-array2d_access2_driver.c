#include <stdio.h>

int access(int i, int j);

int main()
{

    if (access(0, 0) != 1)
    {
        printf("Error (0,0): %d\n", access(0, 0));
        return 1;
    }

    // if (access(0, 1) != 2)
    // {
    //     printf("Error (0,1): %d\n", access(0, 1));
    //     return 2;
    // }

    // if (access(1, 0) != 3)
    // {
    //     printf("Error (0,2): %d\n", access(1, 0));
    //     return 3;
    // }

    // if (access(1, 1) != 4)
    // {
    //     printf("Error (1,1): %d\n", access(1, 1));
    //     return 4;
    // }

    return 0;
}
