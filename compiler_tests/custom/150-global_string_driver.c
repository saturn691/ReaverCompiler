int f(int n);

int main()
{
    if (f(0) != 'H') return 1;
    if (f(1) != 'e') return 2;
    if (f(2) != 'l') return 3;
    if (f(3) != 'l') return 4;
    if (f(4) != 'o') return 5;
    if (f(5) != ',') return 6;
    if (f(6) != ' ') return 7;
    if (f(7) != 'W') return 8;
    if (f(8) != 'o') return 9;
    if (f(9) != 'r') return 10;
    if (f(10) != 'l') return 11;
    if (f(11) != 'd') return 12;
    if (f(12) != '!') return 13;
    return 0;
}
