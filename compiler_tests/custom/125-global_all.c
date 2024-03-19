char _char = 127;
unsigned _unsigned = 4294967295u;
int _int = 2147483647;
float _float = 3.402823e+38f;
double _double = 1.7976931348623158e+308;

int check()
{
    if (_char != 127)
        return 1;

    if (_unsigned != 4294967295u)
        return 2;

    if (_int != 2147483647)
        return 3;

    if (_float != 3.402823e+38f)
        return 4;

    if (_double != 1.7976931348623158e+308)
        return 5;

    return 0;
}
