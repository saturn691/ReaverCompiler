int f(int x)
{
    int counter = 0;
    int i;

    for (i = 0; i < 10; i++)
    {
        if (i == x)
        {
            break;
        }
        counter++;
    }

    return counter;
}
