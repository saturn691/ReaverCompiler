
int f();

int main()
{
    int x[2];
    x[1]=13;
    printf("%d\n", f(x));
    return !(f(x)==13);
}
