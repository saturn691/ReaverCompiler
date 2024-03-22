int f()
{
    int x;
    int *y = &x;
    *y = 10;
    return x;
}
