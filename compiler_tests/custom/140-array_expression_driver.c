int f(int x);

int main()
{
    if (f(0) != 1)
    {
        return 1;
    }

    if (f(1) != 2)
    {
        return 2;
    }

    if (f(2) != 3)
    {
        return 3;
    }

    if (f(3) != 4)
    {
        return 4;
    }

    return 0;
}
