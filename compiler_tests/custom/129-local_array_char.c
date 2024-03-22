int check()
{
    char e[4] = {1, 2, 3, 4};

    if (e[0] != 1)
        return 1;

    if (e[1] != 2)
        return 2;

    if (e[2] != 3)
        return 3;

    if (e[3] != 4)
        return 4;

    return 0;
}
