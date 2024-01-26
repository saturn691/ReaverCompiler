
int g();

int main()
{
    int x;
    printf("g()=%d\n", g());
    return !(g()=='h');
}
