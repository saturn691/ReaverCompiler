int access(int i, int j)
{
    int f[2][2];
    f[0][0] = 1;
    f[0][1] = 2;
    f[1][0] = 3;
    f[1][1] = 4;

    return f[i][j];
}
