int f(int x)
{
    int counter = 0;
    int i = 0;

    while (i < 10)
    {
        if (i >= x)
        {
            i++;
            continue;
        }
        counter++;
        i++;
    }

    return counter;
}
