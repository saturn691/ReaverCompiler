int sum(int x[3][3][3])
{
    int i;
    int j;
    int k;
    int s = 0;

    for (i = 0; i < 3; i = i + 1)
    {
        for (j = 0; j < 3; j = j + 1)
        {
            for (k = 0; k < 3; k = k + 1)
            {
                s = s + x[i][j][k];
            }
        }
    }

    return s;
}
