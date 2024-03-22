float e[4] = {1.0f, 2.0f, 3.0f, 4.0f};

int check()
{
    if (e[0] != 1.0f)
        return 1;

    if (e[1] != 2.0f)
        return 2;

    if (e[2] != 3.0f)
        return 3;

    if (e[3] != 4.0f)
        return 4;

    return 0;
}
