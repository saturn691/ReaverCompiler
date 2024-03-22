int f()
{
    int x[10];
    int *y=&x[5];
    x[5]=13;
    return *y;
}
