int f(int x)
{
    int arr[sizeof(int) + 1];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;

    return arr[x];
}
