int f(int x)
{
    int counter = 0;

    while (1)
    {
        if (x <= 0)
        {
            break;
        }
        counter++;
        x--;
    }

    return counter;
}
