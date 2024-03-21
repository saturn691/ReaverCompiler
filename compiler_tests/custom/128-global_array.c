char e[4] = {1, 2, 3, 4};
int f[4] = {1, 2, 3, 4};
float g[4] = {1.0f, 2.0f, 3.0f, 4.0f};
double h[4] = {1.0, 2.0, 3.0, 4.0};

int check()
{
    if (e[0] != 1)
        return 1;

    if (e[1] != 2)
        return 2;

    if (e[2] != 3)
        return 3;

    if (e[3] != 4)
        return 4;

    if (f[0] != 1)
        return 5;

    if (f[1] != 2)
        return 6;

    if (f[2] != 3)
        return 7;

    if (f[3] != 4)
        return 8;

    if (g[0] != 1.0f)
        return 9;

    if (g[1] != 2.0f)
        return 10;

    if (g[2] != 3.0f)
        return 11;

    if (g[3] != 4.0f)
        return 12;

    if (h[0] != 1.0)
        return 13;

    if (h[1] != 2.0)
        return 14;

    if (h[2] != 3.0)
        return 15;

    if (h[3] != 4.0)
        return 16;

    return 0;
}
