int f(int x)
{
    int counter = 0;
    int i = 0;

    do {
        if (i >= x) {
            i++;
            continue;
        }
        counter++;
        i++;
    } while (i < 10);

    return counter;
}
