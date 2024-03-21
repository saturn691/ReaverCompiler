int det(int x[3][3]);

int main()
{
    int x[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    if (det(x) != 0)
    {
        return 1;
    }

    int I[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    if (det(I) != 1)
    {
        return 2;
    }

    return 0;
}
