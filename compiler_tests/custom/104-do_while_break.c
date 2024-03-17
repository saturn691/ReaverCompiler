int f(int x)
{
    int counter = 0;
    int i;

    i = 0;
    do
    {
        if (i >= x)
        {
            break;
        }
        counter++;
        i++;
    } while (1);

    return counter;
}
