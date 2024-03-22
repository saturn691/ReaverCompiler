int sum(int x[2][3][4])
{
    int i;
    int j;
    int k;
    int s = 0;

    for (i = 0; i < 2; i = i + 1)
    {
        for (j = 0; j < 3; j = j + 1)
        {
            for (k = 0; k < 4; k = k + 1)
            {
                s = s + x[i][j][k];
            }
        }
    }

    return s;
}
